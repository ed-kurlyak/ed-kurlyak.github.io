<?php

if (isset ($_POST['u_name']) & isset($_POST['u_email']) & isset($_POST['u_mess']))
{

$var_name =  $_POST[u_name];
$var_email =  $_POST[u_email];
$var_mess =  $_POST[u_mess];



if ($var_name!="" & $var_email!="" & $var_mess!="")
	{
        $note="$var_name(----)$var_email(----)$var_mess(++++)";
	$fp = fopen("cont.txt", 'a');
	flock($fp,2);
        fputs ($fp, $note);
	flock($fp,3);
	fclose($fp);
header("Location: ./cont_fin.html");
	}
else
header("Location: ./cont.html");



}




?>