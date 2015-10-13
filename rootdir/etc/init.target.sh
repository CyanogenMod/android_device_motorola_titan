#/system/bin/sh
RADIO=`getprop ro.boot.radio`
setprop ro.product.model "Moto G 2014"

if [ "$RADIO" = "0x1" ]; then
    # XT1063
    setprop ro.product.device "titan_umts"
    setprop ro.product.name "titan_retuglb"
    setprop ro.build.description "titan_retuglb-user 5.0.2 LXB22.99-24.4 8 release-keys"
    setprop ro.build.fingerprint "motorola/titan_retuglb/titan_umts:5.0.2/LXB22.99-24.4/8:user/release-keys"
    setprop ro.build.product "titan_umts"
    setprop ro.mot.build.customerid "retusa_glb"
    setprop ro.telephony.default_network "0"
    setprop persist.radio.multisim.config ""
elif [ "$RADIO" = "0x5" ]; then
    # XT1068
    setprop ro.product.device "titan_umtsds"
    setprop ro.product.name "titan_retaildsds"
    setprop ro.build.description "titan_retaildsds-user 5.0.2 LXB22.99-24.4 8 release-keys"
    setprop ro.build.fingerprint "motorola/titan_retaildsds/titan_umtsds:5.0.2/LXB22.99-24.4/8:user/release-keys"
    setprop ro.build.product "titan_umtsds"
    setprop ro.mot.build.customerid "retaildsdsall"
    setprop ro.telephony.default_network "0,1"
    setprop persist.radio.multisim.config "dsds"
    setprop persist.radio.dont_use_dsd "true"
    setprop persist.radio.plmn_name_cmp "1"
elif [ "$RADIO" = "0x6" ]; then
    # XT1064
    setprop ro.product.device "titan_umts"
    setprop ro.product.name "titan_retuaws"
    setprop ro.build.description "titan_retuaws-user 5.0.2 LXB22.99-24.4 8 release-keys"
    setprop ro.build.fingerprint "motorola/titan_retuaws/titan_umts:5.0.2/LXB22.99-24.4/8:user/release-keys"
    setprop ro.build.product "titan_umts"
    setprop ro.mot.build.customerid "retusa_aws"
    setprop ro.telephony.default_network "0"
    setprop persist.radio.multisim.config ""
elif [ "$RADIO" = "0x7" ]; then
    # XT1069
    setprop ro.product.device "titan_udstv"
    setprop ro.product.name "titan_retbr_dstv"
    setprop ro.build.description "titan_retbr_dstv-user 5.0.2 LXB22.99-24.4 8 release-keys"
    setprop ro.build.fingerprint "motorola/titan_retbr_dstv/titan_udstv:5.0.2/LXB22.99-24.4/8:user/release-keys"
    setprop ro.build.product "titan_udstv"
    setprop ro.mot.build.customerid "retbr"
    setprop ro.telephony.default_network "0,1"
    setprop persist.radio.multisim.config "dsds"
    setprop persist.radio.dont_use_dsd "true"
    setprop persist.radio.plmn_name_cmp "1"
fi
