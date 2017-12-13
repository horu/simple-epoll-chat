
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <stddef.h>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <string>
#include <thread>

#include <utils/Exception.h>
#include <utils/Process.h>

using namespace utils;

const char Process::LOCK_FILE_NAME[] = "/chat_server.pid";
FileDescriptor Process::lock_file_ = -1;

void utils::Process::BecomeDaemon() {
    // Fork off the parent process
    pid_t pid = fork();
    if (pid < 0) {
        throw Exception("fork", strerror(errno));
    }

    //The parent terminate
    if (pid > 0) {
        exit(0);
    }

    // The current process becomes session leader
    pid_t set_id_result = setsid();
    if (set_id_result < 0) {
        throw Exception("setid", strerror(errno));
    }

    // ignore and handle signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // Fork off for the second time to ensure that process get rid of the session leading process
    pid = fork();
    if (pid < 0) {
        throw Exception("fork", strerror(errno));
    }

    //The parent terminate
    if (pid > 0) {
        exit(0);
    }

    // Set new file permissions
    umask(0);

    // Change the working directory to the root directory or another appropriated directory
    int chdir_result = chdir("/");
    if (chdir_result) {
        throw Exception("chdir /", strerror(errno));
    }

    // Close all open file descriptors
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }

    // Open the log file
    openlog("chat_server", LOG_PID, LOG_DAEMON);
}



void Process::BecomeSigleInstance(const std::string& lock_file_dir, bool force_start) {
    // If the lock file is already locked, this means another instance is running
    std::string lock_file = lock_file_dir + LOCK_FILE_NAME;
    lock_file_ = open(lock_file.c_str(), O_CREAT | O_RDWR, 0666);
    if (lock_file_ == -1) {
        throw Exception(std::string("open ") + lock_file, strerror(errno));
    }

    for (size_t attempt_counter = 0; attempt_counter < 20; attempt_counter++) {
        if (Lock()) {
            // This is the single instance, success start
            return;
        }
        // Another instance is running
        else if (force_start) {
            KillLockerProcess();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            // Start error
            throw Exception("lock", "Another instance is running");
        }
    }
    throw Exception("lock", std::string("Can't unlock ") + lock_file);
}

bool Process::Lock() {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = 0;
    lock.l_len = 2;

    return fcntl(lock_file_, F_SETLK, &lock) == 0;
}

void Process::KillLockerProcess() {
    // Detect lock info, lock.l_pid is the locker pid
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = 0;
    lock.l_len = 0;
    lock.l_pid = 0;
    int get_lock_result = fcntl(lock_file_, F_GETLK, &lock);
    if (get_lock_result) {
        throw Exception("lock", strerror(errno));
    }

    if (lock.l_type == F_UNLCK) {
        return; // lock_file already is unlocked
    }

    // Send the signal to the locker process
    int kill_result = kill(lock.l_pid, SIGKILL);
    if (kill_result && errno != ESRCH) {
        throw Exception("kill", strerror(errno));
    }
    // The signal is sent or the locker process not found
}

