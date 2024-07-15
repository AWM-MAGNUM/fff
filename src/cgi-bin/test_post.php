#!/usr/bin/php-cgi
<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    if (isset($_POST["name"])) {
        $cookie_name = "user";
        $cookie_value = $_POST["name"];
        setcookie($cookie_name, $cookie_value, time() + 3600); // Cookie set to expire in 1 hour (3600 seconds)
        echo "Cookie has been set with the value: " . $cookie_value;
        
        // Redirect to another file after setting the cookie
        header("Location: /cgi-bin/res.php");
        exit; // Ensure that subsequent code is not executed after redirection
    } else {
        echo "Please enter a name before submitting.";
    }
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Set Cookie and Redirect</title>
</head>
<body> 
    <h2>Set Cookie and Redirect</h2>
    <form method="POST" action="">
        <label for="name">Enter your name:</label>
        <input type="text" id="name" name="name" required>
        <input type="submit" name="sub" value="Submit">
    </form>
</body>
</html>
