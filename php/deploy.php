<?php

# Use github webhook to update git repo on server.
# NOTE! Need to set github secret in GH_TOKEN env var.
# NOTE! Need to set github meta url in GH_META_API_URL env var.

# @TODO
# - only deploy on push on release branch.
# - Enable ip checking.
# - Add logging.
# - Security audit.
# - use $_SERVER['HTTP_X_GITHUB_DELIVERY'].

# Get used client ip and filter it.
$ip = $_SERVER['HTTP_CLIENT_IP'] ? $_SERVER['HTTP_CLIENT_IP'] : ($_SERVER['HTTP_X_FORWARDED_FOR'] ? $_SERVER['HTTP_X_FORWARDED_FOR'] : $_SERVER['REMOTE_ADDR']);
$filtered_ip = filter_var($ip, FILTER_VALIDATE_IP);

# Check for deployment permissions.
# Exit if deployment is not allowed.
if (
	(!validUserAgent()) OR
	(!validEvent()) OR
	(!validSignatureHash()) OR
	(!validGithubIp())
) {
	# Notify about denied try.
	shell_exec("./jarvis msg \"Someone tried to deploy:%0A[<i>$filtered_ip</i>]\"");
	exit(1);
}

# Update website and notify.
# NOTE! Need to import public key in gpg keychain and trust it.
echo shell_exec("git pull --verify-signature 2>&1 && ./jarvis msg \"Website deployed:%0A[<i>$filtered_ip</i>]%0A{<i>$(git rev-parse --verify HEAD)</i>}\"");
echo "Website deployed!";

################################################
# Helper functions.

function getSslPage($url) {
	# Return on empty url.
	if (empty($url)) {
		return '';
	}

	# Create curl handler with encoded url.
	$ch = curl_init($url);

	# Set some options.
	$options = array(
		CURLOPT_RETURNTRANSFER => TRUE,
		CURLOPT_USERAGENT => 'curl'	
	);
	curl_setopt_array($ch, $options);

	# Run request and save result.
	$result = curl_exec($ch);

	# Return empty string if error occured.
	if (curl_errno($ch) OR curl_getinfo($ch, CURLINFO_HTTP_CODE) !== 200) {
		return '';	
	}

	# Close curl handler.
	curl_close($ch);

	# Return result page.
	return $result;
}

function validGithubIp() {
	# Client ip.
	$ip = $_SERVER['HTTP_CLIENT_IP'] ? $_SERVER['HTTP_CLIENT_IP'] : ($_SERVER['HTTP_X_FORWARDED_FOR'] ? $_SERVER['HTTP_X_FORWARDED_FOR'] : $_SERVER['REMOTE_ADDR']);

	# Get allowed ips.
	$json_data = json_decode(getSslPage(getenv('GH_META_API_URL')), TRUE, 6);
	$webhook_ips = $json_data['hooks'];

    # @TODO - parse ips in cidr notation.

	# @TODO - Check if client ip is in range.

	return TRUE;
}

function validSignatureHash() {
	$secret = getenv('GH_TOKEN');
	$payload = file_get_contents('php://input');

	if (empty($secret) OR empty($_SERVER['HTTP_X_HUB_SIGNATURE'])) {
		return FALSE;
	}

	return hash_equals('sha1='.hash_hmac('sha1', $payload, $secret), $_SERVER['HTTP_X_HUB_SIGNATURE']);
}

function validUserAgent() {
	if (!isset($_SERVER['HTTP_USER_AGENT'])) {
		return FALSE;
	}
	
	# @TODO - does the random part change for a webhook?
	return substr($_SERVER['HTTP_USER_AGENT'],0,16) === 'GitHub-Hookshot/';
}

function validEvent() {
	if (!isset($_SERVER['HTTP_X_GITHUB_EVENT'])) {
		return FALSE;
	}
		
	return $_SERVER['HTTP_X_GITHUB_EVENT'] === 'push';
}
