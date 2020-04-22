# clog
Use C to complete log moudle, it use map of cstl to store files' handle in order to avoid opening and closing frequently ,and clog print formatted data into stdout or file by using macro(log and log_to_file).

clog's level is defined according to syslog:
    LOG_ERR, LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG. It can set the lowest print level when init clog

## compile


1. git clone https://github.com/zach0zhang/clog.git --recursive

    Now you can add source files(cstl_lib.c, cstl_rb.c, cstl_map.c ,log.c and their related header files) into project and compile them.

    If you do not want to do that, just go to step 2, and compile them to a link library.

2. autoreconf -fiv
3. ./configure --host=arm-linux-gnueabihf --prefix=$PWD/install/ CFLAHS="-std=gnu99" 

    (take arm-linux-gnueabihf-gcc to compile project and store the result files in ./install/ as an example)
3. make clean; make; make install
