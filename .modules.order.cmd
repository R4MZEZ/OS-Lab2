cmd_/home/roman/Desktop/ioctl/modules.order := {   echo /home/roman/Desktop/ioctl/mymodule.ko; :; } | awk '!x[$$0]++' - > /home/roman/Desktop/ioctl/modules.order
