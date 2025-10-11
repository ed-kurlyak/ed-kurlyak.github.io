<?php

if (isset($_POST['u_mess']))
{

$var_mess =  $_POST[u_mess];



if ($var_mess!="")
	{
    

	$to      = 'ed.kurlyak@ukr.net';
	$subject = 'the subject';
	$message = $var_mess;
	$headers = 'From: SOFTWARE SOLUTIONS <webmaster@example.com>' . "\r\n" .
    'Reply-To: webmaster@example.com' . "\r\n" .
    'X-Mailer: PHP/' . phpversion();

	mail($to, $subject, $message, $headers);


	header("Location: ./cont_fin.html");
	}
else
header("Location: ./cont.html");



}




?>