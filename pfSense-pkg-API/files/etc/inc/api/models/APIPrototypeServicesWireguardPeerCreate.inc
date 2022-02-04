<?php
//   Copyright 2022 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");
require_once('wireguard/includes/wg.inc');


class APIPrototypeServicesWireguardPeerCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-wireguard"];
        $this->change_note = "Added Wireguard peer via API";
    }

    public function action() {
        $this->__init_config();
        $this->config["installedpackages"]["wireguard"]["peers"]["item"][] = $this->validated_data;
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__convert_addresses();
        $validation = wg_do_peer_post($this->initial_data);
        $this->validated_data = $validation["pconfig"];
        $this->id = $validation['tuns_to_sync'];

        # Return a generic error if there was an issue and add the errors to the data field
        if (!empty($validation["input_errors"])) {
            $this->errors[] = APIResponse\get(1, $validation["input_errors"]);
        }
        # Otherwise, add our peer for application if wg is enabled
        else {
            if (wg_is_service_running() and $validation['changes']) {
                wg_apply_list_add('tunnels', $validation['tuns_to_sync']);
            }
        }
    }

    public function apply() {
        if (wg_is_service_running()) {
            $tunnels = wg_apply_list_get('tunnels');
            $sync = wg_tunnel_sync($tunnels, true, true);
        }
    }

    # Converts the config addresses values to expected UI values
    private function __convert_addresses() {
        foreach ($this->initial_data["allowedips"]["row"] as $id=>$address) {
            $this->initial_data["address".$id] = $address["address"];
            $this->initial_data["address_subnet".$id] = $address["mask"];
            $this->initial_data["address_descr".$id] = $address["descr"];
        }
        unset($this->initial_data["allowedips"]);
    }

    # Initializes the wireguard configuration
    private function __init_config() {
        if (empty($this->config["installedpackages"]["wireguard"])) {
            $this->config["installedpackages"]["wireguard"] = [];
        }

        if (empty($this->config["installedpackages"]["wireguard"]["peers"])) {
            $this->config["installedpackages"]["wireguard"]["peers"] = [];
            $this->config["installedpackages"]["wireguard"]["peers"]["item"] = [];
        }
    }
}
