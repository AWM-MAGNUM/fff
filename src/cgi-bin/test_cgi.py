#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()  # Pour afficher les erreurs dans le navigateur

print("Content-Type: text/html;charset=utf-8")
print()

# Récupérer les données du formulaire
form = cgi.FieldStorage()

# Extraire les valeurs
nom = form.getvalue('nom')
age = form.getvalue('age')

# Générer la réponse HTML
print("""
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Résultat du Test CGI</title>
</head>
<body>
    <h1>Résultat du Test CGI</h1>
""")

if nom and age:
    print(f"<p>Nom: {nom}</p>")
    print(f"<p>Age: {age}</p>")
else:
    print("<p>Aucune donnée reçue.</p>")

print("""
</body>
</html>
""")

