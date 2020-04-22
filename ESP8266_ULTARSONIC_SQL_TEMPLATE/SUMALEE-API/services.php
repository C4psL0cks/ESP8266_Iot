<?php
require_once "./dbconfig.php";

class Services
{

    private $conn;

    public function __construct()
    {
        $database = new Database();
        $db = $database->dbConnection();
        $this->conn = $db;
    }

    public function runQuery($sql)
    {
        $database = $this->conn->prepare($sql);
        return $database;
    }

    // insert
    public function sensor_update($status,$iot_id)
    {
        try {
            $database = $this->conn->prepare("UPDATE `parking` SET `sensor_status` =:status WHERE `iot_id` = :iot_id");
            $database->execute(array(':status' => $status, ':iot_id' => $iot_id));
            return true;
        } catch (PDOException $e) {
            echo $e->getMessage();
        }

    }
    // select by id
    public function select_by_id($booking_random_id)
    {
        try
        {
            $database = $this->conn->prepare("SELECT * FROM `booking` WHERE `booking_random_id` =:booking_random_id ORDER BY `book_id`");
            $database->bindValue(':booking_random_id', $booking_random_id);
            $database->execute();
            $row = $database->fetch(PDO::FETCH_ASSOC);
            return $row;
        } catch (PDOException $e) {
            echo $e->getMessage();
        }
    }

    // update scan in
    public function update_by_id_scan_out($id_encode, $datetime)
    {
        try
        {
            $database = $this->conn->prepare("UPDATE `booking` SET `scan_out` =:datetime WHERE `book_id` = :book_id");
            $database->execute(array(':datetime' => $datetime, ':book_id' => $id_encode));
            return true;
        } catch (PDOException $e) {
            echo $e->getMessage();
        }
    }

    // update scan out
    public function update_by_id_scan_in($id_encode, $datetime, $reset_check_sum)
    {
        try
        {
            $database = $this->conn->prepare("UPDATE `booking` SET `check_sum` =:check_sum ,`scan_in` =:datetime WHERE `book_id` = :book_id");
            $database->execute(array(':check_sum' => $reset_check_sum, ':datetime' => $datetime, ':book_id' => $id_encode));
            return true;
        } catch (PDOException $e) {
            echo $e->getMessage();
        }
    }

    public function redirect($url)
    {
        header("Location: $url");
    }

}
