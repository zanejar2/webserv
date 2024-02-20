<?php
    if ($_SERVER['REQUEST_METHOD'] === 'GET') {
        $name = $_GET['name'] ?? 'hola';
        $email = $_GET['email'] ?? 'hola';
        echo "Name: $name\n";
        echo "Email: $email\n";
    }
?>
