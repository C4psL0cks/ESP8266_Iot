<?php

	header("Access-Control-Allow-Origin: *");
	header("Content-Type: application/json; charset=UTF-8");
	header("Access-Control-Allow-Methods: POST");
	header("Access-Control-Max-Age: 3600");
	header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

	http_response_code(200);

	// POST input json
	require_once "./services.php";

	// Call services
	$services = new Services();

	//Receive the RAW post data.
	$content = trim(file_get_contents("php://input"));

	//Attempt to decode the incoming RAW post data from JSON.
	$decoded = json_decode($content, true);

	//print_r($decoded);
	echo json_encode($decoded,true);

	$cm = $decoded['cm'];
	$iot_id = $decoded['id_iot'];

	// echo $cm;
	// echo $iot_id;

	try {
		if ($cm < 20) {
			$status = "ไม่ว่าง";
		} else {
			$status = "ว่าง";
		}
		$services->sensor_update($status, $iot_id);
	} catch (PDOException $e) {
		echo $e->getMessage();
	}

?>
