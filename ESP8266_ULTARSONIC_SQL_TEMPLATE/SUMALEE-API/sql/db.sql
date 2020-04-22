-- phpMyAdmin SQL Dump
-- version 4.9.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 21, 2020 at 10:36 AM
-- Server version: 10.4.8-MariaDB
-- PHP Version: 7.3.11

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `look`
--

-- --------------------------------------------------------

--
-- Table structure for table `booking`
--

CREATE TABLE `booking` (
  `book_id` int(11) NOT NULL,
  `booking_random_id` text DEFAULT NULL,
  `car_id` int(11) NOT NULL,
  `park_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `book_date` date NOT NULL,
  `book_time` time NOT NULL,
  `qr_code` text DEFAULT NULL,
  `status_name` text DEFAULT NULL,
  `check_sum` int(11) NOT NULL,
  `scan_in` varchar(200) DEFAULT NULL,
  `scan_out` varchar(200) DEFAULT NULL,
  `created_at` datetime DEFAULT NULL,
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `booking`
--

INSERT INTO `booking` (`book_id`, `booking_random_id`, `car_id`, `park_id`, `user_id`, `book_date`, `book_time`, `qr_code`, `status_name`, `check_sum`, `scan_in`, `scan_out`, `created_at`, `updated_at`) VALUES
(1, '5164865', 1, 1, 1, '2019-12-28', '17:40:08', '/booking/QRcode/0003/2019-12-28_1577529606_qrcode.svg', 'completed', 1, '19-01-2020 17:01:55', '19-01-2020 17:03:36', '2019-12-28 17:40:08', '2019-12-28 11:26:49'),
(2, '1111111', 1, 3, 4, '2019-12-28', '18:05:06', '/booking/QRcode/0003/2019-12-28_1577531106_qrcode.svg', 'completed', 1, '19-01-2020 17:03:50', '19-01-2020 17:07:21', '2019-12-28 18:05:06', '2019-12-28 11:06:21'),
(3, '2222222', 1, 2, 2, '2019-12-28', '18:21:41', '/booking/QRcode/0003/2019-12-28_1577532101_qrcode.svg', 'completed', 0, '2019-12-31 13:13:09', '2019-12-31 19:13:09', '2019-12-28 18:21:41', '2019-12-31 06:23:16'),
(4, '2342342', 3, 3, 3, '2019-12-31', '13:24:03', '/booking/QRcode/0003/2019-12-31_1577773442_qrcode.svg', 'completed', 0, '2019-12-31 19:13:09', '2019-12-31 21:06:09', '2019-12-31 13:24:03', '2019-12-31 06:24:26'),
(5, '4353245', 3, 1, 3, '2019-12-31', '13:45:59', '/booking/QRcode/0003/2019-12-31_1577774759_qrcode.svg', 'canceled', 0, NULL, NULL, '2019-12-31 13:45:59', '2019-12-31 06:50:22'),
(6, '7896890', 1, 1, 1, '2019-12-31', '13:50:28', '/booking/QRcode/0003/2019-12-31_1577775028_qrcode.svg', 'pending', 1, '19-01-2020 17:10:19', '19-01-2020 17:28:24', '2019-12-31 13:50:28', '2019-12-31 06:50:28'),
(7, '7485197', 3, 1, 3, '2019-12-31', '14:32:40', '/booking/QRcode/0003/2019-12-31_1577777560_qrcode.svg', 'canceled', 1, '19-01-2020 17:09:55', '19-01-2020 17:10:06', '2019-12-31 14:32:40', '2019-12-31 07:38:32'),
(8, '4443924', 4, 3, 3, '2019-12-31', '14:38:39', '/booking/QRcode/0003/2019-12-31_1577777918_qrcode.svg', 'canceled', 1, '19-01-2020 17:07:31', '19-01-2020 17:09:45', '2019-12-31 14:38:39', '2019-12-31 07:39:40'),
(9, '3430604', 4, 1, 3, '2019-12-31', '14:39:52', '/booking/QRcode/0003/2019-12-31_1577777992_qrcode.svg', 'canceled', 0, NULL, NULL, '2019-12-31 14:39:52', '2019-12-31 07:41:03'),
(10, '2944903', 4, 2, 3, '2019-12-31', '14:41:09', '/booking/QRcode/0003/2019-12-31_1577778069_qrcode.svg', 'completed', 1, '19-01-2020 17:28:45', '19-01-2020 17:28:55', '2019-12-31 14:41:09', '2020-01-12 03:54:18'),
(11, '6395655', 3, 3, 3, '2020-01-12', '15:10:10', '/booking/QRcode/0003/2020-01-12_1578816608_qrcode.svg', 'pending', 0, NULL, NULL, '2020-01-12 15:10:10', '2020-01-12 08:10:10');

-- --------------------------------------------------------

--
-- Table structure for table `car`
--

CREATE TABLE `car` (
  `car_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `license_plate` text NOT NULL,
  `brand` varchar(255) NOT NULL,
  `model` varchar(255) NOT NULL,
  `color` text NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `car`
--

INSERT INTO `car` (`car_id`, `user_id`, `license_plate`, `brand`, `model`, `color`, `created_at`, `updated_at`) VALUES
(3, 3, 'ดก 3345', 'benz', 'xx', 'ดำ', '2020-01-12 11:44:04', '2020-01-12 04:44:04'),
(4, 3, 'หห 34343', 'isuzu', 'หห', 'แดง', '2020-01-12 11:46:00', '2020-01-12 04:46:00');

-- --------------------------------------------------------

--
-- Table structure for table `parking`
--

CREATE TABLE `parking` (
  `id` int(11) NOT NULL,
  `park_id` text NOT NULL,
  `park_sname` varchar(50) NOT NULL,
  `park_status` varchar(50) NOT NULL,
  `iot_id` text NOT NULL,
  `iot_status` varchar(50) NOT NULL,
  `iot_sname` varchar(255) NOT NULL,
  `sensor_status` varchar(20) NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `parking`
--

INSERT INTO `parking` (`id`, `park_id`, `park_sname`, `park_status`, `iot_id`, `iot_status`, `iot_sname`, `sensor_status`, `created_at`, `updated_at`) VALUES
(1, '001', 'parking001', 'Active', '001', 'Active', 'IOT001', 'ไม่ว่าง', '0000-00-00 00:00:00', '2020-01-12 03:13:23'),
(2, '002', 'parking002', 'Inactive', '002', 'Active', 'IOT002', '', '0000-00-00 00:00:00', '2020-01-11 10:53:48'),
(3, '003', 'parking003', 'Active', '003', 'Active', 'IOT003', '', '0000-00-00 00:00:00', '2020-01-11 10:53:48'),
(4, '004', 'parking004', 'Inactive', '004', 'Active', 'IOT004', '', '0000-00-00 00:00:00', '2020-01-11 10:53:48'),
(5, '005', 'parking005', 'Active', '005', 'Active', 'IOT005', '', '0000-00-00 00:00:00', '2020-01-11 10:53:48'),
(6, '006', 'parking006', 'Inactive', '006', 'Active', 'IOT006', '', '0000-00-00 00:00:00', '2020-01-11 10:53:48');

-- --------------------------------------------------------

--
-- Table structure for table `payment`
--

CREATE TABLE `payment` (
  `pay_id` int(11) NOT NULL,
  `book_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `pay_datetime` datetime DEFAULT NULL,
  `total_hour` decimal(10,2) DEFAULT NULL,
  `price` decimal(10,2) DEFAULT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `user_account`
--

CREATE TABLE `user_account` (
  `user_id` int(11) NOT NULL,
  `username` varchar(255) NOT NULL,
  `password` text NOT NULL,
  `firstname` varchar(255) NOT NULL,
  `lastname` varchar(255) NOT NULL,
  `sex` varchar(50) NOT NULL,
  `email` text NOT NULL,
  `birthday` date DEFAULT NULL,
  `address` text DEFAULT NULL,
  `tel` text DEFAULT NULL,
  `user_type` varchar(50) NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `user_account`
--

INSERT INTO `user_account` (`user_id`, `username`, `password`, `firstname`, `lastname`, `sex`, `email`, `birthday`, `address`, `tel`, `user_type`, `created_at`, `updated_at`) VALUES
(1, 'user001', '123456', 'user', '001', 'male', 'user.test@test.com', '2019-12-01', 'Bangkok', '0852556565', 'user', '2019-12-01 21:15:00', '2019-12-08 05:41:58'),
(2, 'admin', '1234', 'admin', 'admin', 'female', 'admin@admin.com', '2019-12-01', NULL, '0984343321', 'admin', '2019-12-01 00:00:00', '2019-12-31 08:17:44'),
(3, 'mindtest', 'mind1234', 'samalee', 'mind', 'female', 'triplesk@gmail.com', '1994-10-22', 'Chumphon', '0952569967', 'user', '2019-12-05 13:22:13', '2019-12-05 07:30:24'),
(4, 'user02', 'user1234', 'user', '002', 'male', 'user@user.com', '2019-11-25', 'Bangkok', '0987678875', 'user', '2019-12-08 17:42:21', '2019-12-08 10:42:21');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `booking`
--
ALTER TABLE `booking`
  ADD PRIMARY KEY (`book_id`);

--
-- Indexes for table `car`
--
ALTER TABLE `car`
  ADD PRIMARY KEY (`car_id`);

--
-- Indexes for table `parking`
--
ALTER TABLE `parking`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `payment`
--
ALTER TABLE `payment`
  ADD PRIMARY KEY (`pay_id`);

--
-- Indexes for table `user_account`
--
ALTER TABLE `user_account`
  ADD PRIMARY KEY (`user_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `booking`
--
ALTER TABLE `booking`
  MODIFY `book_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=12;

--
-- AUTO_INCREMENT for table `car`
--
ALTER TABLE `car`
  MODIFY `car_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `parking`
--
ALTER TABLE `parking`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- AUTO_INCREMENT for table `payment`
--
ALTER TABLE `payment`
  MODIFY `pay_id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `user_account`
--
ALTER TABLE `user_account`
  MODIFY `user_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
