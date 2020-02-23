1. Run ./gencrt.sh or if you have your own certificate, move to the openssl_client directory, the name is ca.crt,server.crt, server.key, client.crt and client.key.

    The server.crt and client.crt was generate by the same ca.crt in ./gencrt.sh.

    Server side needs ca.crt(to verify client.crt), server.crt, server.key

    Client side needs ca.crt(to verify server.crt), client.crt, client.key

    If you have two ca.crt to generate server.crt and client.crt respectively, client1.crt is generate by ca1.crt and client1.key, server2.crt is generate by ca2.crt and server2.key:

    Client side needs ca2.crt, client1.crt, client1.key.

    Server side needs ca1.crt, server2.crt, server2.key.

    Rename ca2.crt client1.crt client1.key to ca.crt client.crt client.key and run ./genheader.sh.

    Use ca1.crt in openssl s_server -CAfile option.

2. Run ./genheader.sh.

3. Modify this two lines in file openssl_demo.c to your computer server ip and port.

    ```#define OPENSSL_DEMO_TARGET_NAME "192.168.3.196"```

    ```#define OPENSSL_DEMO_TARGET_TCP_PORT 443```


4. Modify thease two lines in file user_config.h to your local Wi-Fi SSID and Password.

    ```#define SSID "HUAWEI001"```

    ```#define PASSWORD ""```

5. Make sure that the computer and ESP8266 are in the same local area network.

6. Run ./gen_misc.sh.

7. Run openssl s_server -CAfile ca.crt -cert server.crt -key server.key -verify 1 -tls1_1 -accept 443.

8. Download bin file to ESP8266.

**ATTENTION**

**1. Make sure the free heap size larger than 30K.**

**2. Make sure the private key length larger than 2048.**

**3. Make sure the fragment size range is between 2048 and 8192.**


openssl s_client -connect 176.122.191.84:443 -CAfile ./ca.crt -cert ./client.crt -key ./client.key -verify 1

CentOS run openssl

1. close firewall or add port, than s_client can access 443
	systemctl stop firewalld.service
	or
	firewall-cmd --add-port=443/tcp

2. firewall-cmd using
	
	#安装启动firewall
	yum install firewalld
	systemctl start firewalld.service
	
	#查询firewall状态
	firewall-cmd --state
	
	#
	firewall-cmd --get-active-zones
	
	#列出所有服务
	firewall-cmd --list-all
	
	#添加开放端口
	firewall-cmd --add-port
	
	#查询服务启动状态
	firewall-cmd --query-service ftp|ssh|samba
	
	#查询所有可用服务
	firewall-cmd --get-service
	
3. systemctl using
	#列出所有可用单元
	systemctl list-unit-files
	
	#列出所有运行中的单元
	systemctl list-units
	
	#列出所有服务
	systemctl list-unit-files --type=service
	
	#启动，停止，重启服务
	systemctl start httpd.service
	systemctl stop firewalld.service
	
	systemctl reboot
	systemctl halt
	systemctl suspend
	systemctl hibernate
	systemctl hybrid-sleep
	
	#运行等级
	Runlevel 0 : 关闭系统
	Runlevel 1 : 救援，维护模式
	Runlevel 3 : 多用户，无图形系统
	Runlevel 4 : 多用户，无图形系统
	Runlevel 5 : 多用户，图形化系统
	Runlevel 6 : 关闭并重启机器
	
	#设置默认运行等级
	systemctl set-default runlevel3.target
	systemctl set-default runlevel5.target
	
	#启动指定运行等级
	systemctl isolate runlevel5.target
	