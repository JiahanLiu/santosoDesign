-- MySQL dump 10.13  Distrib 5.5.41, for debian-linux-gnu (armv7l)
--
-- Host: localhost    Database: wisedb
-- ------------------------------------------------------
-- Server version	5.5.41-0+wheezy1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `contactors`
--

USE wisedb;

DROP TABLE IF EXISTS `contactors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `contactors` (
  `id` int(11) NOT NULL,
  `zone_id` int(11) DEFAULT NULL,
  `rgb_id` mediumint(9) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `contactors`
--

LOCK TABLES `contactors` WRITE;
/*!40000 ALTER TABLE `contactors` DISABLE KEYS */;
INSERT INTO `contactors` VALUES (1,1,25501),(2,1,25502),(3,2,25503),(4,3,25504),(5,4,25505),(6,4,25506),(7,5,25507),(8,6,25508),(9,7,25509),(10,8,255010),(11,9,255011),(12,10,255012),(13,11,255013),(14,10,255014),(15,11,255015),(16,12,255016),(17,13,255017),(18,14,255018);
/*!40000 ALTER TABLE `contactors` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pom`
--

DROP TABLE IF EXISTS `pom`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pom` (
  `id` tinyint(4) NOT NULL,
  `name` varchar(25) DEFAULT NULL,
  `active` tinyint(4) DEFAULT NULL,
  `message_size` mediumint(9) DEFAULT NULL,
  `decoder_a0` tinyint(4) DEFAULT NULL,
  `decoder_a1` tinyint(4) DEFAULT NULL,
  `decoder_a2` tinyint(4) DEFAULT NULL,
  `decoder_a3` tinyint(4) DEFAULT NULL,
  `signal_coeff` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pom`
--

LOCK TABLES `pom` WRITE;
/*!40000 ALTER TABLE `pom` DISABLE KEYS */;
INSERT INTO `pom` VALUES (1,'VT-1',1,768,1,0,0,0,2.06),(2,'VT-17',1,768,0,0,0,0,2.06),(3,'VT-3',0,768,1,1,0,0,2.06),(4,'VT-4',0,768,0,0,1,0,2.06),(5,'VT-5',0,768,1,0,1,0,2.06),(6,'VT-16',0,768,1,0,0,0,2.06),(7,'VT-7',0,768,1,1,1,0,2.06),(8,'VT-8',0,768,1,1,0,0,2.06),(9,'VT-9',0,768,1,0,0,1,2.06),(10,'VT-10',0,768,0,1,0,0,2.06);
/*!40000 ALTER TABLE `pom` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pom_data`
--

DROP TABLE IF EXISTS `pom_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pom_data` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `pom_id` tinyint(4) DEFAULT NULL,
  `insert_date` datetime DEFAULT NULL,
  `insert_date_micro` mediumint(9) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1990 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pom_data`
--

LOCK TABLES `pom_data` WRITE;
/*!40000 ALTER TABLE `pom_data` DISABLE KEYS */;
/*!40000 ALTER TABLE `pom_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pom_data_samples`
--

DROP TABLE IF EXISTS `pom_data_samples`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pom_data_samples` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `pom_data_id` int(11) DEFAULT NULL,
  `sample_type` varchar(10) DEFAULT NULL,
  `phase` char(1) DEFAULT NULL,
  `rms` smallint(6) DEFAULT NULL,
  `min` smallint(6) DEFAULT NULL,
  `max` smallint(6) DEFAULT NULL,
  `0_harm` smallint(6) DEFAULT NULL,
  `1_harm` smallint(6) DEFAULT NULL,
  `2_harm` smallint(6) DEFAULT NULL,
  `3_harm` smallint(6) DEFAULT NULL,
  `4_harm` smallint(6) DEFAULT NULL,
  `5_harm` smallint(6) DEFAULT NULL,
  `6_harm` smallint(6) DEFAULT NULL,
  `7_harm` smallint(6) DEFAULT NULL,
  `8_harm` smallint(6) DEFAULT NULL,
  `9_harm` smallint(6) DEFAULT NULL,
  `10_harm` smallint(6) DEFAULT NULL,
  `11_harm` smallint(6) DEFAULT NULL,
  `12_harm` smallint(6) DEFAULT NULL,
  `13_harm` smallint(6) DEFAULT NULL,
  `s1` smallint(6) DEFAULT NULL,
  `s2` smallint(6) DEFAULT NULL,
  `s3` smallint(6) DEFAULT NULL,
  `s4` smallint(6) DEFAULT NULL,
  `s5` smallint(6) DEFAULT NULL,
  `s6` smallint(6) DEFAULT NULL,
  `s7` smallint(6) DEFAULT NULL,
  `s8` smallint(6) DEFAULT NULL,
  `s9` smallint(6) DEFAULT NULL,
  `s10` smallint(6) DEFAULT NULL,
  `s11` smallint(6) DEFAULT NULL,
  `s12` smallint(6) DEFAULT NULL,
  `s13` smallint(6) DEFAULT NULL,
  `s14` smallint(6) DEFAULT NULL,
  `s15` smallint(6) DEFAULT NULL,
  `s16` smallint(6) DEFAULT NULL,
  `s17` smallint(6) DEFAULT NULL,
  `s18` smallint(6) DEFAULT NULL,
  `s19` smallint(6) DEFAULT NULL,
  `s20` smallint(6) DEFAULT NULL,
  `s21` smallint(6) DEFAULT NULL,
  `s22` smallint(6) DEFAULT NULL,
  `s23` smallint(6) DEFAULT NULL,
  `s24` smallint(6) DEFAULT NULL,
  `s25` smallint(6) DEFAULT NULL,
  `s26` smallint(6) DEFAULT NULL,
  `s27` smallint(6) DEFAULT NULL,
  `s28` smallint(6) DEFAULT NULL,
  `s29` smallint(6) DEFAULT NULL,
  `s30` smallint(6) DEFAULT NULL,
  `s31` smallint(6) DEFAULT NULL,
  `s32` smallint(6) DEFAULT NULL,
  `s33` smallint(6) DEFAULT NULL,
  `s34` smallint(6) DEFAULT NULL,
  `s35` smallint(6) DEFAULT NULL,
  `s36` smallint(6) DEFAULT NULL,
  `s37` smallint(6) DEFAULT NULL,
  `s38` smallint(6) DEFAULT NULL,
  `s39` smallint(6) DEFAULT NULL,
  `s40` smallint(6) DEFAULT NULL,
  `s41` smallint(6) DEFAULT NULL,
  `s42` smallint(6) DEFAULT NULL,
  `s43` smallint(6) DEFAULT NULL,
  `s44` smallint(6) DEFAULT NULL,
  `s45` smallint(6) DEFAULT NULL,
  `s46` smallint(6) DEFAULT NULL,
  `s47` smallint(6) DEFAULT NULL,
  `s48` smallint(6) DEFAULT NULL,
  `s49` smallint(6) DEFAULT NULL,
  `s50` smallint(6) DEFAULT NULL,
  `s51` smallint(6) DEFAULT NULL,
  `s52` smallint(6) DEFAULT NULL,
  `s53` smallint(6) DEFAULT NULL,
  `s54` smallint(6) DEFAULT NULL,
  `s55` smallint(6) DEFAULT NULL,
  `s56` smallint(6) DEFAULT NULL,
  `s57` smallint(6) DEFAULT NULL,
  `s58` smallint(6) DEFAULT NULL,
  `s59` smallint(6) DEFAULT NULL,
  `s60` smallint(6) DEFAULT NULL,
  `s61` smallint(6) DEFAULT NULL,
  `s62` smallint(6) DEFAULT NULL,
  `s63` smallint(6) DEFAULT NULL,
  `s64` smallint(6) DEFAULT NULL,
  `s65` smallint(6) DEFAULT NULL,
  `s66` smallint(6) DEFAULT NULL,
  `s67` smallint(6) DEFAULT NULL,
  `s68` smallint(6) DEFAULT NULL,
  `s69` smallint(6) DEFAULT NULL,
  `s70` smallint(6) DEFAULT NULL,
  `s71` smallint(6) DEFAULT NULL,
  `s72` smallint(6) DEFAULT NULL,
  `s73` smallint(6) DEFAULT NULL,
  `s74` smallint(6) DEFAULT NULL,
  `s75` smallint(6) DEFAULT NULL,
  `s76` smallint(6) DEFAULT NULL,
  `s77` smallint(6) DEFAULT NULL,
  `s78` smallint(6) DEFAULT NULL,
  `s79` smallint(6) DEFAULT NULL,
  `s80` smallint(6) DEFAULT NULL,
  `s81` smallint(6) DEFAULT NULL,
  `s82` smallint(6) DEFAULT NULL,
  `s83` smallint(6) DEFAULT NULL,
  `s84` smallint(6) DEFAULT NULL,
  `s85` smallint(6) DEFAULT NULL,
  `s86` smallint(6) DEFAULT NULL,
  `s87` smallint(6) DEFAULT NULL,
  `s88` smallint(6) DEFAULT NULL,
  `s89` smallint(6) DEFAULT NULL,
  `s90` smallint(6) DEFAULT NULL,
  `s91` smallint(6) DEFAULT NULL,
  `s92` smallint(6) DEFAULT NULL,
  `s93` smallint(6) DEFAULT NULL,
  `s94` smallint(6) DEFAULT NULL,
  `s95` smallint(6) DEFAULT NULL,
  `s96` smallint(6) DEFAULT NULL,
  `s97` smallint(6) DEFAULT NULL,
  `s98` smallint(6) DEFAULT NULL,
  `s99` smallint(6) DEFAULT NULL,
  `s100` smallint(6) DEFAULT NULL,
  `s101` smallint(6) DEFAULT NULL,
  `s102` smallint(6) DEFAULT NULL,
  `s103` smallint(6) DEFAULT NULL,
  `s104` smallint(6) DEFAULT NULL,
  `s105` smallint(6) DEFAULT NULL,
  `s106` smallint(6) DEFAULT NULL,
  `s107` smallint(6) DEFAULT NULL,
  `s108` smallint(6) DEFAULT NULL,
  `s109` smallint(6) DEFAULT NULL,
  `s110` smallint(6) DEFAULT NULL,
  `s111` smallint(6) DEFAULT NULL,
  `s112` smallint(6) DEFAULT NULL,
  `s113` smallint(6) DEFAULT NULL,
  `s114` smallint(6) DEFAULT NULL,
  `s115` smallint(6) DEFAULT NULL,
  `s116` smallint(6) DEFAULT NULL,
  `s117` smallint(6) DEFAULT NULL,
  `s118` smallint(6) DEFAULT NULL,
  `s119` smallint(6) DEFAULT NULL,
  `s120` smallint(6) DEFAULT NULL,
  `s121` smallint(6) DEFAULT NULL,
  `s122` smallint(6) DEFAULT NULL,
  `s123` smallint(6) DEFAULT NULL,
  `s124` smallint(6) DEFAULT NULL,
  `s125` smallint(6) DEFAULT NULL,
  `s126` smallint(6) DEFAULT NULL,
  `s127` smallint(6) DEFAULT NULL,
  `s128` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11035 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pom_data_samples`
--

LOCK TABLES `pom_data_samples` WRITE;
/*!40000 ALTER TABLE `pom_data_samples` DISABLE KEYS */;
/*!40000 ALTER TABLE `pom_data_samples` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `zones`
--

DROP TABLE IF EXISTS `zones`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `zones` (
  `id` int(11) NOT NULL,
  `label` varchar(15) DEFAULT NULL,
  `label_loc` char(1) DEFAULT NULL,
  `rgb_id` mediumint(9) DEFAULT NULL,
  `checked` tinyint(4) DEFAULT NULL,
  `gpio_pin` tinyint(4) DEFAULT NULL,
  `relay` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `zones`
--

LOCK TABLES `zones` WRITE;
/*!40000 ALTER TABLE `zones` DISABLE KEYS */;
INSERT INTO `zones` VALUES (1,'1-2','n',255021,0,36,11),(2,'3','s',255025,0,33,12),(3,'4','n',255026,0,38,13),(4,'5-6','w',255022,0,35,14),(5,'7','w',255027,0,40,15),(6,'8','e',255028,0,37,16),(7,'9','s',255029,0,16,1),(8,'10','n',255030,0,13,2),(9,'11','w',255031,0,18,3),(10,'12-14','s',255024,0,15,4),(11,'13-15','n',255023,0,29,6),(12,'16','s',255032,0,22,5),(13,'17','n',255033,0,32,7),(14,'18','e',255034,0,31,8);
/*!40000 ALTER TABLE `zones` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-04-22 20:21:47
