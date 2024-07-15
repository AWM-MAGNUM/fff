#!/usr/bin/php-cgi
<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    if (isset($_POST["name"])) {
        $name = htmlspecialchars($_POST["name"]);
        echo "Success! Received name: " . $name;
    } else {
        echo "Error! Name variable not received.";
    }
} else {
    echo "Error! Method not allowed.";
}
?>


<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>POST Form Test</title>
</head>
<body>
    <h2>POST Form Test</h2>
    <form method="POST" action="process_post.php">
        <label for="name">Enter your name:</label>
        <input type="text" id="name" name="name">
        <button type="submit">Submit</button>
    </form>
</body>
</html>
