#!/bin/bash

# Author: lbuque
# Github: https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series

RED="\033[31m"      # Error message
GREEN="\033[32m"    # Success message
YELLOW="\033[33m"   # Warning message
BLUE="\033[36m"     # Info message
PLAIN='\033[0m'

# set -euxo pipefail

readonly __version=0.1.1

readonly WORK_DIR=/home/pi/.sx1302_hal

readonly sx1302_hal_name=sx1302_hal
readonly sx1302_hal_url=https://github.com/Lora-net/sx1302_hal/archive/refs/tags/V2.1.0.tar.gz
readonly sx1302_hal_version=2.1.0


colorEcho() {
    echo -e "${1}${@:2}${PLAIN}"
}


checkSystem() {
    uid=$(id -u)
    if [[ $uid -ne 0 ]]; then
        colorEcho $RED " Please execute the script as root" # " 请以root身份执行该脚本"
        exit 1
    fi

    res=$(command -v apt)
    if [[ "$res" = "" ]]; then
        colorEcho $RED "Unsupported Linux systems"
        exit 1
    fi

    res=$(command -v systemctl)
    if [[ "$res" = "" ]]; then
        colorEcho $RED "The system version is too low, please upgrade to the latest version"
        exit 1
    fi

    res=$(command -v wget)
    if [[ "$res" = "" ]]; then
        colorEcho $RED "Missing wget tool"
        colorEcho $YELLOW "Try to install wget tool"
        apt-get install wget
        if [ $? != 0 ]; then
            colorEcho $RED "Failed to install wget tool"
            exit 1
        fi
    fi

    res=$(command -v patch)
    if [[ "$res" = "" ]]; then
        colorEcho $RED "Missing patch tool"
        colorEcho $YELLOW "Try to install patch tool"
        apt-get install patch
        if [ $? != 0 ]; then
            colorEcho $RED "Failed to install patch tool"
            exit 1
        fi
    fi
}


prepare() {
    if [ ! -d "$WORK_DIR" ]; then
        mkdir -p $WORK_DIR
    fi

    if [ ! -d "$WORK_DIR/dist" ]; then
        mkdir -p $WORK_DIR/dist
    fi

    if [ ! -d "$WORK_DIR/bin" ]; then
        mkdir -p $WORK_DIR/bin
    fi
}


download() {
    sx1302_hal_archive=$WORK_DIR/dist/$sx1302_hal_name-$sx1302_hal_version.tar.gz
    if [ ! -f $sx1302_hal_archive ]; then
        wget $sx1302_hal_url -O $sx1302_hal_archive
    fi

    if [ ! -d $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version ]; then
        tar -xvf $WORK_DIR/dist/$sx1302_hal_name-$sx1302_hal_version.tar.gz -C $WORK_DIR
    fi
}


patching() {
    if [ -f $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/.patch ]; then
        return 0
    fi

    local readonly oragw_hal_c_patch=$(cat <<EOF
diff --git a/libloragw/src/loragw_hal.c b/libloragw/src/loragw_hal.c
index ffc8ec0..504bb42 100644
--- a/libloragw/src/loragw_hal.c
+++ b/libloragw/src/loragw_hal.c
@@ -1093,6 +1093,7 @@ int lgw_start(void) {
     dbg_init_random();
 
     if (CONTEXT_COM_TYPE == LGW_COM_SPI) {
+#if 0
         /* Find the temperature sensor on the known supported ports */
         for (i = 0; i < (int)(sizeof I2C_PORT_TEMP_SENSOR); i++) {
             ts_addr = I2C_PORT_TEMP_SENSOR[i];
@@ -1116,7 +1117,7 @@ int lgw_start(void) {
             printf("ERROR: no temperature sensor found.\n");
             return LGW_HAL_ERROR;
         }
-
+#endif
         /* Configure ADC AD338R for full duplex (CN490 reference design) */
         if (CONTEXT_BOARD.full_duplex == true) {
             err = i2c_linuxdev_open(I2C_DEVICE, I2C_PORT_DAC_AD5338R, &ad_fd);
@@ -1285,14 +1286,14 @@ int lgw_receive(uint8_t max_pkt, struct lgw_pkt_rx_s *pkt_data) {
         nb_pkt_left = nb_pkt_fetched - max_pkt;
         printf("WARNING: not enough space allocated, fetched %d packet(s), %d will be left in RX buffer\n", nb_pkt_fetched, nb_pkt_left);
     }
-
+#if 0
     /* Apply RSSI temperature compensation */
     res = lgw_get_temperature(&current_temperature);
     if (res != LGW_I2C_SUCCESS) {
         printf("ERROR: failed to get current temperature\n");
         return LGW_HAL_ERROR;
     }
-
+#endif
     /* Iterate on the RX buffer to get parsed packets */
     for (nb_pkt_found = 0; nb_pkt_found < ((nb_pkt_fetched <= max_pkt) ? nb_pkt_fetched : max_pkt); nb_pkt_found++) {
         /* Get packet and move to next one */
EOF
)

    echo "$oragw_hal_c_patch" | patch -d $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version -p0 libloragw/src/loragw_hal.c --verbose
    touch $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/.patch
}


compile() {
    cd $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version
    make all
    cd -
}


install() {
    systemctl stop lora_pkt_fwd.service
    systemctl disable lora_pkt_fwd.service

    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/libloragw/test_loragw_* $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/tools/reset_lgw.sh $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/packet_forwarder/lora_pkt_fwd $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/util_net_downlink/net_downlink $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/util_chip_id/chip_id $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/util_boot/boot $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/util_spectral_scan/spectral_scan $WORK_DIR/bin

    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/packet_forwarder/global_conf.json.sx1250.* $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/packet_forwarder/global_conf.json.sx1255.* $WORK_DIR/bin
    cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/packet_forwarder/global_conf.json.sx1257.* $WORK_DIR/bin

    sed -i "s/SX1302_RESET_PIN=23/SX1302_RESET_PIN=7/" $WORK_DIR/bin/reset_lgw.sh
}


raspiConfig() {
    ret=0
    if [ -c /dev/spidev0.1 ]; then
        sed -i '/.*dtoverlay=spi0*/c\dtoverlay=spi0-1cs' /boot/config.txt
    elif [ -c /dev/spidev0.0 ]; then
        ret=1
    else
        if [ `cat /boot/config.txt | grep "dtoverlay=spi0"` ]; then
            sed -i '/.*dtoverlay=spi0*/c\dtoverlay=spi0-1cs' /boot/config.txt
        else
            echo "dtoverlay=spi0-1cs" >> /boot/config.txt
        fi
    fi
    echo $ret
}


getEthName() {
    local interface=`ifconfig | grep enx | awk '{print $1}'`
    if [ "$interface" = "" ]; then
        interface=`ifconfig | grep eth0 | awk '{print $1}'`
    fi
    echo $interface
}


getGatewayID() {
    local readonly GWID_MIDFIX="FFFE"
    local interface=`getEthName`
    if [ "$interface" = "" ]; then
        echo "001122"$GWID_MIDFIX"334455"
    else
        interface=${interface%%:*}
        local readonly GWID_BEGIN=$(ip link show $interface | awk '/ether/ {print $2}' | awk -F\: '{print $1$2$3}')
        local readonly GWID_END=$(ip link show $interface | awk '/ether/ {print $2}' | awk -F\: '{print $4$5$6}')
        echo $GWID_BEGIN$GWID_MIDFIX$GWID_END
    fi
}


eu868Config() {
    local readonly global_conf=$(cat <<EOF
{
    "SX130x_conf": {
        "com_type": "SPI",
        "com_path": "/dev/spidev0.0",
        "lorawan_public": true,
        "clksrc": 0,
        "antenna_gain": 0, /* antenna gain, in dBi */
        "full_duplex": false,
        "precision_timestamp": {
            "enable": false,
            "max_ts_metrics": 255,
            "nb_symbols": 1
        },
        "radio_0": {
            "enable": true,
            "type": "SX1250",
            "freq": 867500000,
            "rssi_offset": -215.4,
            "rssi_tcomp": {
                "coeff_a": 0,
                "coeff_b": 0,
                "coeff_c": 20.41,
                "coeff_d": 2162.56,
                "coeff_e": 0
            },
            "tx_enable": true,
            "tx_freq_min": 863000000,
            "tx_freq_max": 870000000,
            "tx_gain_lut": [
                {
                    "rf_power": 12,
                    "pa_gain": 0,
                    "pwr_idx": 15
                },
                {
                    "rf_power": 13,
                    "pa_gain": 0,
                    "pwr_idx": 16
                },
                {
                    "rf_power": 14,
                    "pa_gain": 0,
                    "pwr_idx": 17
                },
                {
                    "rf_power": 15,
                    "pa_gain": 0,
                    "pwr_idx": 19
                },
                {
                    "rf_power": 16,
                    "pa_gain": 0,
                    "pwr_idx": 20
                },
                {
                    "rf_power": 17,
                    "pa_gain": 0,
                    "pwr_idx": 22
                },
                {
                    "rf_power": 18,
                    "pa_gain": 1,
                    "pwr_idx": 1
                },
                {
                    "rf_power": 19,
                    "pa_gain": 1,
                    "pwr_idx": 2
                },
                {
                    "rf_power": 20,
                    "pa_gain": 1,
                    "pwr_idx": 3
                },
                {
                    "rf_power": 21,
                    "pa_gain": 1,
                    "pwr_idx": 4
                },
                {
                    "rf_power": 22,
                    "pa_gain": 1,
                    "pwr_idx": 5
                },
                {
                    "rf_power": 23,
                    "pa_gain": 1,
                    "pwr_idx": 6
                },
                {
                    "rf_power": 24,
                    "pa_gain": 1,
                    "pwr_idx": 7
                },
                {
                    "rf_power": 25,
                    "pa_gain": 1,
                    "pwr_idx": 9
                },
                {
                    "rf_power": 26,
                    "pa_gain": 1,
                    "pwr_idx": 11
                },
                {
                    "rf_power": 27,
                    "pa_gain": 1,
                    "pwr_idx": 14
                }
            ]
        },
        "radio_1": {
            "enable": true,
            "type": "SX1250",
            "freq": 868500000,
            "rssi_offset": -215.4,
            "rssi_tcomp": {
                "coeff_a": 0,
                "coeff_b": 0,
                "coeff_c": 20.41,
                "coeff_d": 2162.56,
                "coeff_e": 0
            },
            "tx_enable": true
        },
        "chan_multiSF_0": {
            "enable": true,
            "radio": 1,
            "if": -400000
        },
        "chan_multiSF_1": {
            "enable": true,
            "radio": 1,
            "if": -200000
        },
        "chan_multiSF_2": {
            "enable": true,
            "radio": 1,
            "if": 0
        },
        "chan_multiSF_3": {
            "enable": true,
            "radio": 0,
            "if": -400000
        },
        "chan_multiSF_4": {
            "enable": true,
            "radio": 0,
            "if": -200000
        },
        "chan_multiSF_5": {
            "enable": true,
            "radio": 0,
            "if": 0
        },
        "chan_multiSF_6": {
            "enable": true,
            "radio": 0,
            "if": 200000
        },
        "chan_multiSF_7": {
            "enable": true,
            "radio": 0,
            "if": 400000
        },
        "chan_Lora_std": {
            "enable": true,
            "radio": 1,
            "if": -200000,
            "bandwidth": 250000,
            "spread_factor": 7,
            "implicit_hdr": false,
            "implicit_payload_length": 17,
            "implicit_crc_en": false,
            "implicit_coderate": 1
        },
        "chan_FSK": {
            "enable": true,
            "radio": 1,
            "if": 300000,
            "bandwidth": 125000,
            "datarate": 50000
        }
    },
    "gateway_conf": {
        "gateway_ID": "$1",
        /* change with default server address/ports */
        "server_address": "$2",
        "serv_port_up": $3,
        "serv_port_down": $3,
        /* adjust the following parameters for your network */
        "keepalive_interval": 10,
        "stat_interval": 30,
        "push_timeout_ms": 100,
        /* forward only valid packets */
        "forward_crc_valid": true,
        "forward_crc_error": false,
        "forward_crc_disabled": false,
        /* GPS configuration */
        //"gps_tty_path": "/dev/ttyS0",
        /* GPS reference coordinates */
        // "ref_latitude": 0.0,
        //"ref_longitude": 0.0,
        //"ref_altitude": 0,
        /* Beaconing parameters */
        "beacon_period": 0,
        "beacon_freq_hz": 869525000,
        "beacon_datarate": 9,
        "beacon_bw_hz": 125000,
        "beacon_power": 14,
        "beacon_infodesc": 0
    },
    "debug_conf": {
        "ref_payload": [
            {
                "id": "0xCAFE1234"
            },
            {
                "id": "0xCAFE2345"
            }
        ],
        "log_file": "loragw_hal.log"
    }
}
EOF
)
    echo "$global_conf" > $WORK_DIR/bin/global_conf.json
}


us915Config() {
    local readonly global_conf=$(cat <<EOF
{
    "SX130x_conf": {
        "com_type": "SPI",
        "com_path": "/dev/spidev0.0",
        "lorawan_public": true,
        "clksrc": 0,
        "antenna_gain": 0, /* antenna gain, in dBi */
        "full_duplex": false,
        "fine_timestamp": {
            "enable": false,
            "mode": "all_sf" /* high_capacity or all_sf */
        },
        "sx1261_conf": {
            "spi_path": "/dev/spidev0.1",
            "rssi_offset": 0, /* dB */
            "spectral_scan": {
                "enable": false,
                "freq_start": 903900000,
                "nb_chan": 8,
                "nb_scan": 2000,
                "pace_s": 10
            },
            "lbt": {
                "enable": false /* LBT for 500 Khz channels is not supported */
            }
        },
        "radio_0": {
            "enable": true,
            "type": "SX1250",
            "freq": 904300000,
            "rssi_offset": -215.4,
            "rssi_tcomp": {"coeff_a": 0, "coeff_b": 0, "coeff_c": 20.41, "coeff_d": 2162.56, "coeff_e": 0},
            "tx_enable": true,
            "tx_freq_min": 923000000,
            "tx_freq_max": 928000000,
            "tx_gain_lut":[
                {"rf_power": 12, "pa_gain": 0, "pwr_idx": 15},
                {"rf_power": 13, "pa_gain": 0, "pwr_idx": 16},
                {"rf_power": 14, "pa_gain": 0, "pwr_idx": 17},
                {"rf_power": 15, "pa_gain": 0, "pwr_idx": 19},
                {"rf_power": 16, "pa_gain": 0, "pwr_idx": 20},
                {"rf_power": 17, "pa_gain": 0, "pwr_idx": 22},
                {"rf_power": 18, "pa_gain": 1, "pwr_idx": 1},
                {"rf_power": 19, "pa_gain": 1, "pwr_idx": 2},
                {"rf_power": 20, "pa_gain": 1, "pwr_idx": 3},
                {"rf_power": 21, "pa_gain": 1, "pwr_idx": 4},
                {"rf_power": 22, "pa_gain": 1, "pwr_idx": 5},
                {"rf_power": 23, "pa_gain": 1, "pwr_idx": 6},
                {"rf_power": 24, "pa_gain": 1, "pwr_idx": 7},
                {"rf_power": 25, "pa_gain": 1, "pwr_idx": 9},
                {"rf_power": 26, "pa_gain": 1, "pwr_idx": 11},
                {"rf_power": 27, "pa_gain": 1, "pwr_idx": 14}
            ]
        },
        "radio_1": {
            "enable": true,
            "type": "SX1250",
            "freq": 905000000,
            "rssi_offset": -215.4,
            "rssi_tcomp": {"coeff_a": 0, "coeff_b": 0, "coeff_c": 20.41, "coeff_d": 2162.56, "coeff_e": 0},
            "tx_enable": false
        },
        "chan_multiSF_All": {"spreading_factor_enable": [ 5, 6, 7, 8, 9, 10, 11, 12 ]},
        "chan_multiSF_0": {"enable": true, "radio": 0, "if": -400000},  /* Freq : 903.9 MHz*/
        "chan_multiSF_1": {"enable": true, "radio": 0, "if": -200000},  /* Freq : 904.1 MHz*/
        "chan_multiSF_2": {"enable": true, "radio": 0, "if":  0},       /* Freq : 904.3 MHz*/
        "chan_multiSF_3": {"enable": true, "radio": 0, "if":  200000},  /* Freq : 904.5 MHz*/
        "chan_multiSF_4": {"enable": true, "radio": 1, "if": -300000},  /* Freq : 904.7 MHz*/
        "chan_multiSF_5": {"enable": true, "radio": 1, "if": -100000},  /* Freq : 904.9 MHz*/
        "chan_multiSF_6": {"enable": true, "radio": 1, "if":  100000},  /* Freq : 905.1 MHz*/
        "chan_multiSF_7": {"enable": true, "radio": 1, "if":  300000},  /* Freq : 905.3 MHz*/
        "chan_Lora_std":  {"enable": true, "radio": 0, "if":  300000, "bandwidth": 500000, "spread_factor": 8,						/* Freq : 904.6 MHz*/
                           "implicit_hdr": false, "implicit_payload_length": 17, "implicit_crc_en": false, "implicit_coderate": 1},
        "chan_FSK":       {"enable": false, "radio": 1, "if":  300000, "bandwidth": 125000, "datarate": 50000}						/* Freq : 868.8 MHz*/
    },

    "gateway_conf": {
        "gateway_ID": "AA555A0000000000",
        /* change with default server address/ports */
        "server_address": "localhost",
        "serv_port_up": 1730,
        "serv_port_down": 1730,
        /* adjust the following parameters for your network */
        "keepalive_interval": 10,
        "stat_interval": 30,
        "push_timeout_ms": 100,
        /* forward only valid packets */
        "forward_crc_valid": true,
        "forward_crc_error": false,
        "forward_crc_disabled": false,
        /* GPS configuration */
        "gps_tty_path": "/dev/ttyS0",
        /* GPS reference coordinates */
        "ref_latitude": 0.0,
        "ref_longitude": 0.0,
        "ref_altitude": 0,
        /* Beaconing parameters */
        "beacon_period": 0, /* disable class B beacon */
        "beacon_freq_hz": 869525000,
        "beacon_datarate": 9,
        "beacon_bw_hz": 125000,
        "beacon_power": 14,
        "beacon_infodesc": 0
    },

    "debug_conf": {
        "ref_payload":[
            {"id": "0xCAFE1234"},
            {"id": "0xCAFE2345"}
        ],
        "log_file": "loragw_hal.log"
    }
}

EOF
)
    echo "$global_conf" > $WORK_DIR/bin/global_conf.json
}


as923Config() {
    local readonly global_conf=$(cat <<EOF
{
    "SX130x_conf": {
        "com_type": "SPI",
        "com_path": "/dev/spidev0.0",
        "lorawan_public": true,
        "clksrc": 0,
        "antenna_gain": 0, /* antenna gain, in dBi */
        "full_duplex": false,
        "precision_timestamp": {
            "enable": false,
            "max_ts_metrics": 255,
            "nb_symbols": 1
        },
        "radio_0": {
            "enable": true,
            "type": "SX1250",
            "freq": 922300000,
            "rssi_offset": -215.4,
            "rssi_tcomp": {"coeff_a": 0, "coeff_b": 0, "coeff_c": 20.41, "coeff_d": 2162.56, "coeff_e": 0},
            "tx_enable": true,
            "tx_freq_min": 920000000,
            "tx_freq_max": 924000000,
            "tx_gain_lut": [
                {"rf_power": 0, "pa_gain": 0, "pwr_idx": 0},
                {"rf_power": 12, "pa_gain": 0, "pwr_idx": 15},
                {"rf_power": 13, "pa_gain": 0, "pwr_idx": 16},
                {"rf_power": 14, "pa_gain": 0, "pwr_idx": 17},
                {"rf_power": 15, "pa_gain": 0, "pwr_idx": 19},
                {"rf_power": 16, "pa_gain": 0, "pwr_idx": 20},
                {"rf_power": 17, "pa_gain": 0, "pwr_idx": 22},
                {"rf_power": 18, "pa_gain": 1, "pwr_idx": 1},
                {"rf_power": 19, "pa_gain": 1, "pwr_idx": 2},
                {"rf_power": 20, "pa_gain": 1, "pwr_idx": 3},
                {"rf_power": 21, "pa_gain": 1, "pwr_idx": 4},
                {"rf_power": 22, "pa_gain": 1, "pwr_idx": 5},
                {"rf_power": 23, "pa_gain": 1, "pwr_idx": 6},
                {"rf_power": 24, "pa_gain": 1, "pwr_idx": 9}
            ]
        },
        "radio_1": {
            "enable": true,
            "type": "SX1250",
            "freq": 923100000,
            "rssi_offset": -215.4,
            "rssi_tcomp": {
                "coeff_a": 0,
                "coeff_b": 0,
                "coeff_c": 20.41,
                "coeff_d": 2162.56,
                "coeff_e": 0
            },
            "tx_enable": true
        },
        "chan_multiSF_All": {"spreading_factor_enable": [ 5, 6, 7, 8, 9, 10, 11, 12 ]},
        "chan_multiSF_0": {"enable": true, "radio": 0, "if": -300000},
        "chan_multiSF_1": {"enable": true, "radio": 0, "if": -100000},
        "chan_multiSF_2": {"enable": true, "radio": 0, "if":  100000},
        "chan_multiSF_3": {"enable": true, "radio": 0, "if":  300000},
        "chan_multiSF_4": {"enable": true, "radio": 1, "if": -300000},
        "chan_multiSF_5": {"enable": true, "radio": 1, "if": -100000},
        "chan_multiSF_6": {"enable": true, "radio": 1, "if":  100000},
        "chan_multiSF_7": {"enable": true, "radio": 1, "if":  300000},
        "chan_Lora_std":  {"enable": true, "radio": 0, "if":  300000, "bandwidth": 500000, "spread_factor": 8,
                           "implicit_hdr": false, "implicit_payload_length": 17, "implicit_crc_en": false, "implicit_coderate": 1},
        "chan_FSK":       {"enable": false, "radio": 1, "if":  300000, "bandwidth": 125000, "datarate": 50000}
    
    },
    "gateway_conf": {
        "gateway_ID": "$1",
        /* change with default server address/ports */
        "server_address": "$2",
        "serv_port_up": $3,
        "serv_port_down": $3,
        /* adjust the following parameters for your network */
        "keepalive_interval": 10,
        "stat_interval": 30,
        "push_timeout_ms": 100,
        /* forward only valid packets */
        "forward_crc_valid": true,
        "forward_crc_error": false,
        "forward_crc_disabled": false,
        /* GPS configuration */
        //"gps_tty_path": "/dev/ttyS0",
        /* GPS reference coordinates */
        // "ref_latitude": 0.0,
        //"ref_longitude": 0.0,
        //"ref_altitude": 0,
        /* Beaconing parameters */
        "beacon_period": 0,
        "beacon_freq_hz": 923400000,
        "beacon_datarate": 9,
        "beacon_bw_hz": 125000,
        "beacon_power": 14,
        "beacon_infodesc": 0
    },
    "debug_conf": {
        "ref_payload": [
            {"id": "0xCAFE1234"},
            {"id": "0xCAFE2345"}
        ],
        "log_file": "loragw_hal.log"
    }
}

EOF
)
    echo "$global_conf" > $WORK_DIR/bin/global_conf.json
}


daemon() {
    local readonly lora_pkt_fwd_service_file=$(cat <<EOF
[Unit]
Description=LoRa Packet Forwarder
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
WorkingDirectory=$WORK_DIR/bin
ExecStart=$WORK_DIR/bin/lora_pkt_fwd -c $WORK_DIR/bin/global_conf.json
Restart=always
RestartSec=30
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=lora_pkt_fwd

[Install]
WantedBy=multi-user.target
EOF
)
    echo "$lora_pkt_fwd_service_file" > /etc/systemd/system/lora_pkt_fwd.service
    systemctl daemon-reload
    systemctl enable lora_pkt_fwd.service
    systemctl restart lora_pkt_fwd.service
    sudo cp $WORK_DIR/$sx1302_hal_name-$sx1302_hal_version/tools/systemd/lora_pkt_fwd.conf /etc/rsyslog.d
    sudo systemctl restart rsyslog
    # reboot
}


help() {
    local readonly usage="Script to automatically deploy packet_forwarder

Usage: sudo ./sx1302_hal.sh [-s server] [-p port] [-f frequency] [--eui eui] [--verbose] [-o output]
       sudo ./sx1302_hal.sh
       ./sx1302_hal.sh -v
       ./sx1302_hal.sh --help

Option:
  -s --server  Gateway Server address. The default value is eu1.cloud.thethings.network.
  -p --port  Gateway Server port. The default value is 1700.
  -f --frequency  The frequency band of lorawan supports eu868, us915, as923. The default value is eu868.
  -eui  Gateway EUI. The default is produced by the mac address of eth0.
  -o --output  Output gateway information.

Miscellaneous options:
  --verbose  Output extra information about the work being done.

  -v --version  Output version info.
  --help  Output this help.

Report bugs to https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/issues."
    echo -e "$usage"
}


version() {
    local readonly version_info="sx1302_hal.sh $__version
Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
Written by lbuque"
    echo -e "$version_info"
}


showInfo() {
    echo
    colorEcho $GREEN "packet_forwarder installed successfully!!! "
    colorEcho $GREEN "==========================================="
    colorEcho $GREEN "Installation path: $WORK_DIR"
    colorEcho $GREEN "application path: $WORK_DIR/bin/lora_pkt_fwd"
    colorEcho $GREEN "Configuration file path: $WORK_DIR/bin/global_conf.json"
    colorEcho $GREEN "Systemctl file path: /etc/systemd/system/lora_pkt_fwd.service"

    colorEcho $GREEN "Gateway EUI: $1"
    colorEcho $GREEN "Gateway Server address: $2"
    colorEcho $GREEN "Gateway Server port: $3"

    if [ "$4" != "" ]; then
        echo "Gateway EUI: $1" > $4
        echo "Gateway Server address: $2" >> $4
        echo "Gateway Server port: $3" >> $4
    fi
}


main() {
    TEMP=`getopt -o s:p:f:o:v --long server:,port:,frequency:,eui:,output:,verbose,version,help -n $0 -- "$@"`
    if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
    eval set -- "$TEMP"
    local eui=`getGatewayID`
    eui=${eui^^}
    local server_address="eu1.cloud.thethings.network"
    local server_port=1700
    local frequency_plan="eu868"
    local output_file=$WORK_DIR/.output
    while true ; do
        case "$1" in
            -s|--server)
                server_address=$2
                shift 2
            ;;
            -p|--port)
                server_port=$2
                shift 2
            ;;
            --eui)
                eui=${$2^^}
                shift 2
            ;;
            -f|--frequency)
                frequency_plan=$2
                case "$frequency_plan" in
                    eu868|us915|as923)
                    ;;
                    *)
                        colorEcho $RED "Wrong frequency plan($frequency_plan), Please choose eu868, us915, as923!"
                        exit 1
                    ;;
                esac
                shift 2
            ;;
            -o|--output)
                output_file=$2
                shift 2
            ;;
            --verbose) set -euxo pipefail ; shift ;;
            -v|--version) version; exit 0 ;;
            --help) help; exit 0 ;;
            --) shift ; break ;;
            *) echo "Internal error!" ; exit 1 ;;
        esac
    done

    checkSystem
    prepare
    download
    patching
    compile
    install
    ret=`raspiConfig`
    case "$frequency_plan" in
        eu868)
            eu868Config $eui $server_address $server_port
        ;;
        us915)
            us915Config $eui $server_address $server_port
        ;;
        as923)
            as923Config $eui $server_address $server_port
        ;;
        *)
            colorEcho $RED "Wrong frequency plan($frequency_plan), Please choose eu868, us915, as923!"
            exit 1
        ;;
    esac
    daemon
    showInfo $eui $server_address $server_port $output_file
    if [ "$ret" = "0" ]; then
        echo
        colorEcho $BLUE "Take effect after restarting the system! ! !"
    fi
}

main $@
