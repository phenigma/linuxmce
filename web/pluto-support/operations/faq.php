<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;	
$out='<p>No FAQ\'s yet.</p>';
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>