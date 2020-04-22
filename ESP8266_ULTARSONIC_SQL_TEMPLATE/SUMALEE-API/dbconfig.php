<?php
class Database
{
    // public
    // private $host = "localhost";
    // private $db_name = "id12401785_sumalee";
    // private $username = "id12401785_root";
    // private $password = "sumaleeroot";
    // public $conn;

    // private
    private $host = "localhost";
    private $db_name = "sumalee";
    private $username = "root";
    private $password = "root";
    public $conn;

    public function dbConnection()
    {

        $this->conn = null;
        try
        {
            $this->conn = new PDO("mysql:host=" . $this->host . ";dbname=" . $this->db_name, $this->username, $this->password);
            $this->conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $this->conn->exec("set names utf8");
            date_default_timezone_set('Asia/Bangkok');
        } catch (PDOException $exception) {
            echo "Connection error: " . $exception->getMessage();
        }

        return $this->conn;
    }

}

?>
