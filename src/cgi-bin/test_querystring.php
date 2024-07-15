#!/usr/bin/php-cgi

<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Construire la query string à partir des données POST
    $queryString = http_build_query($_POST);
    
    // URL de redirection avec query string
    $redirectUrl = "http://localhost:9002/cgi-bin/process_post.php?" . $queryString;
    
    // Rediriger vers l'URL avec query string
    header("Location: " . $redirectUrl);
    exit();
} else {
    echo "Error! Method not allowed.";
}
?>
