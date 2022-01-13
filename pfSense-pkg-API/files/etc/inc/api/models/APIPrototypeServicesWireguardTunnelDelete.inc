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

class APIPrototypeServicesWireguardTunnelDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-wireguard"];
        $this->change_note = "Deleted Wireguard tunnel via API";
    }

    public function action() {
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Loop through each tunnel and check if this is the one to delete
        foreach ($this->config["installedpackages"]["wireguard"]["tunnels"]["item"] as $id=>$tunnel) {
            if ($this->initial_data["name"] === $tunnel["name"]) {
                $this->id = $id;
                break;
            }
        }

        $apply = wg_delete_tunnel($this->initial_data["name"]);

        if ($apply["input_errors"]) {
            $this->errors[] = APIResponse\get(1, $apply["input_errors"]);
        }
    }

    public function apply() {
        if (wg_is_service_running()) {
            $tunnels = wg_apply_list_get('tunnels');
            $sync = wg_tunnel_sync($tunnels, true, true);
        }
    }
}
