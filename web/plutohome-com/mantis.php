<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
	<link rel="stylesheet" type="text/css" href="http://localhost/mantis/css/default.css" />
	<script type="text/javascript" language="JavaScript">
		if(document.layers) {document.write("<style>td{padding:0px;}</style>")}
	</script>
	<meta http-equiv="Content-type" content="text/html;charset=windows-1252" />
	<meta http-equiv="Pragma" content="no-cache" />
	<meta http-equiv="Cache-Control" content="no-cache" />
	<meta http-equiv="Pragma-directive" content="no-cache" />
	<meta http-equiv="Cache-Directive" content="no-cache" />
	<meta http-equiv="Expires" content="0" />
	<title>Mantis</title>
	<script type="text/JavaScript" src="javascript/common.js"></script>
</head>
<body>
<div class="center"><span class="pagetitle">Mantis</span></div>
<br /><div align="center">Welcome to the Issue Tracker.<br /></div>
<!-- Login Form BEGIN -->
<br />
<div align="center">
<form name="login_form" method="post" action="http://localhost/mantis/login_page.php">
<table class="width50" cellspacing="1">
<tr>
	<td class="form-title">
		Login	</td>
	<td class="right">
		</td>
</tr>
<tr class="row-1">
	<td class="category" width="25%">
		Username	</td>
	<td width="75%">
		<input type="text" name="username" size="32" maxlength="32" />
	</td>
</tr>
<tr class="row-2">
	<td class="category">
		Password	</td>
	<td>
		<input type="password" name="password" size="16" maxlength="32" />
	</td>
</tr>
<tr class="row-1">
	<td class="category">
		Save Login	</td>
	<td>
		<input type="checkbox" name="perm_login" />
	</td>
</tr>
<tr>
	<td class="center" colspan="2">
		<input type="submit" class="button" value="Login" />
	</td>
</tr>
</table>
</form>
</div>

<br /><div align="center">[&nbsp;<a href="signup_page.php">Signup for a new account</a>&nbsp;]&nbsp;[&nbsp;<a href="lost_pwd_page.php">Lost your password?</a>&nbsp;]</div><div class="warning" align="center"><p><font color="red"><strong>WARNING:</strong> You should disable the default "administrator" account or change its password.</font></p></div><div class="warning" align="center">
<p><font color="red"><strong>WARNING:</strong> Admin directory should be removed.</font></p>
</div>

<!-- Autofocus JS -->
<script type="text/javascript" language="JavaScript">
window.document.login_form.username.focus();
</script>

	<br />
	<hr size="1" />
	<span class="timer"><a href="http://www.mantisbt.org/">Mantis 0.19.1</a></span>
	<address>Copyright &copy; 2000 - 2004 Mantis Group</address>
	<address><a href="mailto:webmaster@plutohome.com">webmaster@plutohome.com</a></address>
	2 total queries executed.<br />
	2 unique queries executed.<br />
</body>
</html>
