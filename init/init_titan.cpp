/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"

#define ISMATCH(a,b)    (!strncmp(a,b,PROP_VALUE_MAX))

void gsm_properties();

void vendor_load_properties()
{
    char platform[PROP_VALUE_MAX];
    char radio[PROP_VALUE_MAX];
    char device[PROP_VALUE_MAX];
    char devicename[PROP_VALUE_MAX];
    int rc;

    rc = property_get("ro.board.platform", platform);
    if (!rc || !ISMATCH(platform, ANDROID_TARGET))
        return;

    property_get("ro.boot.radio", radio);

    property_set("ro.product.model", "Moto G 2014");

    if (ISMATCH(radio, "0x1")) {
        /* XT1063 */
        property_set("ro.product.device", "titan_umts");
        property_set("ro.build.description", "titan_retuglb-user 6.0 MPB24.65-34 31 release-keys");
        property_set("ro.build.fingerprint", "motorola/titan_retuglb/titan_umts:6.0/MPB24.65-34/31:user/release-keys");
        property_set("ro.build.product", "titan_umts");
        property_set("ro.mot.build.customerid", "retusa_glb");
        property_set("ro.telephony.default_network", "0");
        property_set("persist.radio.multisim.config", "");
    } else if (ISMATCH(radio, "0x5")) {
        /*XT1068 */
        property_set("ro.product.device", "titan_umtsds");
        property_set("ro.build.description", "titan_retaildsds-user 6.0 MPB24.65-34 31 release-keys");
        property_set("ro.build.fingerprint", "motorola/titan_retaildsds/titan_umtsds:6.0/MPB24.65-34/31:user/release-keys");
        property_set("ro.build.product", "titan_umtsds");
        property_set("ro.mot.build.customerid", "retaildsdsall");
        property_set("ro.telephony.default_network", "0,1");
        property_set("ro.telephony.ril.config", "simactivation,sim2gsmonly");
        property_set("persist.radio.multisim.config", "dsds");
        property_set("persist.radio.dont_use_dsd", "true");
        property_set("persist.radio.plmn_name_cmp", "1");
    } else if (ISMATCH(radio, "0x6")) {
        /* XT1064 */
        property_set("ro.product.device", "titan_umts");
        property_set("ro.build.description", "titan_retuaws-user 6.0 MPB24.65-34 31 release-keys");
        property_set("ro.build.fingerprint", "motorola/titan_retuaws/titan_umts:6.0/MPB24.65-34/31:user/release-keys");
        property_set("ro.build.product", "titan_umts");
        property_set("ro.mot.build.customerid", "retusa_aws");
        property_set("ro.telephony.default_network", "0");
        property_set("persist.radio.multisim.config", "");
    } else if (ISMATCH(radio, "0x7")) {
        /* XT1069 */
        property_set("ro.product.device", "titan_udstv");
        property_set("ro.build.description", "titan_retbr_dstv-user 6.0 MPB24.65-34 31 release-keys");
        property_set("ro.build.fingerprint", "motorola/titan_retbr_dstv/titan_udstv:6.0/MPB24.65-34/31:user/release-keys");
        property_set("ro.build.product", "titan_udstv");
        property_set("ro.mot.build.customerid", "retbr");
        property_set("ro.telephony.default_network", "0,1");
        property_set("ro.telephony.ril.config", "simactivation,sim2gsmonly");
        property_set("persist.radio.multisim.config", "dsds");
        property_set("persist.radio.dont_use_dsd", "true");
        property_set("persist.radio.plmn_name_cmp", "1");
    }
    property_get("ro.product.device", device);
    strlcpy(devicename, device, sizeof(devicename));
    INFO("Found radio id %s setting build properties for %s device\n", radio, devicename);
}
