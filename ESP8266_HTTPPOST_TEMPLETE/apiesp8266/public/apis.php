<?php


    header("Access-Control-Allow-Origin: *");
    header("Content-Type: application/json; charset=UTF-8");
    header("Access-Control-Allow-Methods: GET");
    header("Access-Control-Max-Age: 3600");
    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

	http_response_code(200);

    require_once '../services/services.php';

    $Services = new Services();

    $data = array();


    $content = file_get_contents("php://input");
    $data = json_decode($content,TRUE);

    // print_r($data);
    // echo $data['temperature'];
    // echo $data['value2'];
    // echo $data['value3'];

    echo json_encode($data);

	if(isset($data['temperature'])){
		$Services->insert_temp($data['temperature']);
	}

?>