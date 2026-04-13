## Download the image
- https://www.raspberrypi.com/software/
- ping -4 <hostname>.local <!-- without -4 returned ipv6 -->

### Issues
- Couldnt connect was trying via wsl and wasnt working
- add a empty file named ssh with no file extension 
- add wpa_supplicant.conf file
    ``` conf
    ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
        update_config=1
        country=<US>
        network={
            ssid="WIFI_NAME"
            psk="WIFI_PASSWORD"
        }
    ```