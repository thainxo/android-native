1. enable "INIT" in configuration of linuxdeploy
2. create script /etc/rc.local
3. create script /etc/rc.d/wifi.local

```sh
WLAN="wlan0"
AP="ap0"
AP_TYPE="managed"

PREFIX_IP="192.168.49"
GW_IP="${PREFIX_IP}.1"
MASK_IP="${PREFIX_IP}.0"
SUB_MASK=24
ROUTE_TABLE=97
CACHE_DNS_HOURS=1h

is_ap_exist() {
    local type=$(iw dev ${AP} info 2>/dev/null | grep -e "type ${AP_TYPE}")
    local is_exist=0
    if [ -z "${type}" ]; then
        is_exist=1
    fi
    return ${is_exist}
}

is_ap_set_ip() {
    local ip=$(ip -4 addr show ${AP} | grep -e "${GW_IP}")
    local is_set=0
    if [ -z "${ip}" ]; then
        is_set=1
    fi
    return ${is_set}
}

kill_process() {
    local process="$1"
    local args="$2"
    pid=$(ps -ef 2>/dev/null | grep -e "${process} .* ${args}" | awk "\$4 ~ /^${process}$/{print \$1}" 2>/dev/null)

    kill -9 ${pid}
}

delete_iptables_rule() {
    iptables-legacy -t nat -D POSTROUTING -s ${MASK_IP}/${SUB_MASK} ! -o ${AP} -j MASQUERADE
    iptables-legacy -D FORWARD -i ${AP} -s ${MASK_IP}/${SUB_MASK} -j ACCEPT
    iptables-legacy -D FORWARD -i ${WLAN} -d ${MASK_IP}/${SUB_MASK} -j ACCEPT
}

add_iptables_rule() {
    iptables-legacy -t nat -I POSTROUTING -s ${MASK_IP}/${SUB_MASK} ! -o ${AP} -j MASQUERADE
    iptables-legacy -I FORWARD -i ${AP} -s ${MASK_IP}/${SUB_MASK} -j ACCEPT
    iptables-legacy -I FORWARD -i ${WLAN} -d ${MASK_IP}/${SUB_MASK} -j ACCEPT
}

enable_hotspot() {
    if is_ap_exist; then
        iw dev ${AP} del
    fi
    iw dev ${WLAN} interface add ${AP} type managed
    ip link set dev ${AP} up
    if ! is_ap_set_ip; then
        ip addr add ${GW_IP}/${SUB_MASK} dev ${AP}
    fi

    ip route add ${MASK_IP}/${SUB_MASK} dev ${AP} table ${ROUTE_TABLE}
    echo 1 > /proc/sys/net/ipv4/conf/${WLAN}/forwarding 
    echo 1 > /proc/sys/net/ipv4/ip_forward

    kill_process hostapd "/etc/hostapd/hostapd.conf"
    sleep 3
    hostapd -B /etc/hostapd/hostapd.conf

    sleep 1
    kill_process dnsmasq "${AP}"
    dnsmasq -i ${AP} --dhcp-authoritative --no-negcache --user=root --no-resolv --no-hosts --no-poll --dhcp-option-force=43,ANDROID_METERED --server=8.8.8.8 --dhcp-range=${PREFIX_IP}.10,${PREFIX_IP}.200,${CACHE_DNS_HOURS}

    delete_iptables_rule
    add_iptables_rule
}

while true; do
    state=$(cat /sys/class/net/wlan0/operstate)
    if [ ! "${state}" = "up" ]; then
        if [ "${state}" = "dormant" ]; then
            ip link set dev ${WLAN} down
        fi
        ip link set dev ${WLAN} up
    else
        state=$(cat /sys/class/net/ap0/operstate)
        if [ ! "${state}" = "up" ]; then
            enable_hotspot
            sleep 10
        fi
    fi
    sleep 300
done


```
# Command lines
## list wifi devices
```sh
iw dev
```
```sh
phy#0
        Unnamed/non-netdev interface
                wdev 0x2
                addr aa:7c:01:d9:a7:d0
                type P2P-device
        Interface wlan0
                ifindex 9
                wdev 0x1
                addr a8:7c:01:d9:a7:d0
                ssid VNTP_ext_2.4G
                type managed
```
## list information of wifi interface
```sh
iw dev wlan0 info
```
```sh
Interface wlan0
        ifindex 9
        wdev 0x1
        addr a8:7c:01:d9:a7:d0
        ssid VNTP_ext_2.4G
        type managed
        wiphy 0
```
