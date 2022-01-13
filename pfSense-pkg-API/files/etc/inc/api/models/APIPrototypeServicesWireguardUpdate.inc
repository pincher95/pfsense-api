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

class APIPrototypeServicesWireguardUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-wireguard"];
        $this->change_note = "Updated Wireguard settings via API";
    }

    public function action() {
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->initial_data["enable"] = ($this->initial_data["enable"] === "on") ? true : $this->initial_data["enable"];

        $validation = wg_do_settings_post($this->initial_data);
        $this->validated_data = $validation["pconfig"];

        # Return a generic error if there was an issue and add the errors to the data field
        if (!empty($validation["input_errors"])) {
            $this->errors[] = APIResponse\get(1, $validation["input_errors"]);
        }
        # Otherwise, add our tunnel for application if wg is enabled
        else {
            if (empty($input_errors) && $validation['changes']) {
                wg_toggle_wireguard();
            }
        }
    }
}
