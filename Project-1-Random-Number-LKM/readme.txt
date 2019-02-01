Project 1 - Loadable Kernel Module

Tìm hiểu và thực hiện: 
Huỳnh Kim Ninh
1612484
CQ2016/31

Mã nguồn được viết trên Visual Studio Code
Build trên Ubuntu 18.04
Có xử lý đồng bộ

Cách cài đặt module và test:

Build module và chương trình test: "make"
Cài module: "sudo insmod generateRandom.ko"
Chạy test: "sudo ./test"
Kiểm tra kernel log: "tail -f /var/log/kernel.log"
	hoặc "dmesg"
Gỡ module: "sudo rmmod generateRandom"
Dọn dẹp các file build: "make clean"