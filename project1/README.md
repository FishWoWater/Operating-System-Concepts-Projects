## Brief Introduction For Kernel Module
### Features
* *do not execute sequentially*(do not have a main function), use `init` and `exit` instead.
* *do not have automatic cleanup*(resources must be released manually)
* *do not have `printf()` function*, since that is for the user space
* *can be interruppted and accessed by multiple users at the same time, so consistent behavior must be ensured.
* *more CPU cycles will be assigned to the kernel module *
* *do not have floating-point support* since it is kernel code that uses traps to transitions from integer to fp
### Makefile
make -C argument: specify the directory which it will enters(here the source code directory)
make M=$(PWD) (M is the variable to be passed on to the Makefile in -C directory so that it will jump back to current directory)
### Specify parameters
`module_param(name, type, permission)`(the `type` specifies the type(e.g. charp) and permission can be specified as (S_INUGO))
### where to find the output
* `/sys/module/$(moduleName)`
* `/var/log/kern.log` (use `tail -f /var/log/kern.log`)
* `/proc` (simply use `cat /proc/modules | grep simple` to grep the output)

