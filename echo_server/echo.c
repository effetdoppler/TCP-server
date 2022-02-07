#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#define BUFFER_SIZE 512

void rewrite(int fd, const void *buf, size_t count)
{
    ssize_t res = write(fd, buf, count);
    //If the return value of write() is smaller than its third argument, you must call write() a
    //gain in order to have the rest of the data written. Repeat this until all the data has been sent
    if (res != count)
    {
        char * buff  = buf;
        while(res != count)
        {
            // If an error occurs, exit the program with an error message
            if (res == -1)
                err(EXIT_FAILURE, "write function has failed");
            res = write(fd, buf, count);
            buff = buff + res;
            count = count - res;
            res = write(fd, buff, count);
        }
    }
}

void echo(int fd_in, int fd_out)
{
    char buffer[BUFFER_SIZE];
    while(1)
    {
        //Read some data from fd_in.
        ssize_t r = read(fd_in, buffer, BUFFER_SIZE);
        //When all the data has been read (r = 0), you must break the loop and return from the function
        if (r == 0)
            return;
        //If an error occurs (r = -1), you must exit the program with an error message
        if (r == -1)
            err(EXIT_FAILURE, "read function has failed");
        //When some data is in the buffer, you must send it to fd_out. To do so, use the rewrite() function
        rewrite(fd_out, buffer, r);
    }
}