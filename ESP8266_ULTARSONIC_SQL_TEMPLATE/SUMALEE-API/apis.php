<?php

    header("Access-Control-Allow-Origin: *");
    header("Content-Type: application/json; charset=UTF-8");
    header("Access-Control-Allow-Methods: GET");
    header("Access-Control-Max-Age: 3600");
    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

    http_response_code(200);

    require_once "./services.php";

    $Services = new Services();
    $data = array();

    if (isset($_GET['text'])) {

        $text = $_GET['text'];
        $select = $Services->select_by_id($text);
        //print_r($select);

        if ($select['booking_random_id'] == $text) {
            //echo "ok";
            $id_encode = $select['book_id'];
            //echo "id_encode:" . $id_encode . "<br>";
            $datetime = date("d-m-Y H:i:s");
            //echo "datetime:" . $datetime . "<br>";
            $check_sum = $select['check_sum'];
            //echo "check_sum:" . $check_sum . "<br>";

            if ($check_sum) {   // ถ้า check_sum = 1 แสดงว่า มีการ scan_in แล้วให้ ทำการอัพเดจ scan_out แทน
                $update = $Services->update_by_id_scan_out($id_encode, $datetime);

            } else {            // ถ้า check_sum = 0 แสดงว่า ไม่มีการสแกนเข้าและออก ให้ทำการอัพเดจ scan in และทำการ set สถานะ check_sum เพื่อระบุว่า ทำการ scan_in แล้ว
                $reset_check_sum = 1;
                $update = $Services->update_by_id_scan_in($id_encode, $datetime, $reset_check_sum);
            }

            if ($update) {
                if ($select) {
                    $data['status'] = 'success';
                    //$data['result'] = $select;
                }
                //returns data as JSON format
                echo json_encode($data);
            }
        } else {
            $data['status'] = 'error';
            //returns data as JSON format
            echo json_encode($data);
        }
    }

?>