<?php

require_once '../db/dbconfig.php';

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

    public function insert_temp($temp)
    {
        try
        {
            $database = $this->conn->prepare("INSERT INTO `data`(`temp`) VALUES (:temp)");
            $database->bindparam(':temp', $temp);
            $database->execute();
            return true;
        } catch (PDOException $e) {
            echo $e->getMessage();
        }
    }
}
