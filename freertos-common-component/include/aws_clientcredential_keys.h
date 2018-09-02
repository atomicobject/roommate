#include <stddef.h>
/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
static const char clientcredentialCLIENT_CERTIFICATE_PEM[] =
"-----BEGIN CERTIFICATE-----\n"
"MIIDWjCCAkKgAwIBAgIVAOw0pZbslt4QIHHXFJR+S1uGv2fzMA0GCSqGSIb3DQEB\n"
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xODA4MjEwMTE3\n"
"MjNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCmLKN9FnbRjN9KHw0z\n"
"XnLvA1dJqfveWbSyunLadcWqZj6zq35WuzFq38OUP4Bf2z9epOMRlYWkW2zkKQpX\n"
"SJRt/CJiSi/vi03IE3qSBIdfWgIUAic5OSGsHWokPkCNeq0Y4Z+jJBweX1CotuqZ\n"
"Nm44fUdHiR6mXuq/epkDEXNvlqzH/3UW5BAwjNotiHegU3okw5ZNutrgJbQiCbuy\n"
"XMxXqKyWITyoY2++6q9IUbDDrpCwwRPKiyD0QQVH0EGP5f5jrw4jLAE/hUiWO7t0\n"
"aJ8mbUbTNMWP5bW/t3VXPKIqL7WrDczpj659ISg/4Q9BENhNJ/1F+wDu4a6jLI2u\n"
"/iovAgMBAAGjYDBeMB8GA1UdIwQYMBaAFP1tAgxDOnjTtgrwv/0RppQjP4KlMB0G\n"
"A1UdDgQWBBRrhQSJWdEOCAHX5hcHcQ1wxHTLYzAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEALwus/xQXxEqykSMedBzgonrw\n"
"uhtHHPwMs/i6WGZ+asXtRovt8LeQK0PTENu7yTmt9xthS3XBd6/YNLq+VS+aiTEw\n"
"xWO59tIZ2R2/erpDik8wL6l12d9jDJaqzwujhjGXnGR0HOP71+H5k8WuLVxrpZKL\n"
"5L79hkZHtCS7BKWn/CpQ8LKSmeu58ej0gisRBaf9q+yR8BR0o4mvnGHGpv5cv98d\n"
"WOYoyhUAdIjXg2SK5PgXshgZ/BhHuw47LUzk92VOnLZ69sJdzvcorg3bWJEzlzpH\n"
"/8Y4ssw6kUuBlqUEvD12fTANVfBoFRqZ3Gdxp3pJIeo71Lb3SrdijBodizl4VQ==\n"
"-----END CERTIFICATE-----\n";

/*
 * PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 * This is required if you're using JITR, since the issuer (Certificate
 * Authority) of the client certificate is used by the server for routing the
 * device's initial request. (The device client certificate must always be
 * sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html,
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set the below pointer to NULL.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
static const char *clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM = NULL;

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----"
 * "...base64 data..."
 * "-----END RSA PRIVATE KEY-----";
 */
static const char clientcredentialCLIENT_PRIVATE_KEY_PEM[] =
"-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEowIBAAKCAQEApiyjfRZ20YzfSh8NM15y7wNXSan73lm0srpy2nXFqmY+s6t+\n"
"Vrsxat/DlD+AX9s/XqTjEZWFpFts5CkKV0iUbfwiYkov74tNyBN6kgSHX1oCFAIn\n"
"OTkhrB1qJD5AjXqtGOGfoyQcHl9QqLbqmTZuOH1HR4kepl7qv3qZAxFzb5asx/91\n"
"FuQQMIzaLYh3oFN6JMOWTbra4CW0Igm7slzMV6isliE8qGNvvuqvSFGww66QsMET\n"
"yosg9EEFR9BBj+X+Y68OIywBP4VIlju7dGifJm1G0zTFj+W1v7d1VzyiKi+1qw3M\n"
"6Y+ufSEoP+EPQRDYTSf9RfsA7uGuoyyNrv4qLwIDAQABAoIBAH9bbuGLufzYbrms\n"
"Gr3oAUZV1SxL7xF2xxrD8wqjgJ8cRaoGIYnvZex3GrogAQ92RZcP4Ee1xPs2Zrpn\n"
"Cdnjcn7o4qFaImsA5NgWtbX7apUVssoK7Gza8Ies85BbdbX483lCUssB/dr5SiRE\n"
"LIDISqlYuqyE1szDqj6eg27qjQJTyGZttu8z4IbaS/9b98hKEvJVbF/TYJRdTT2u\n"
"TfWSIKLuX9ZiW+SynXTNBN4iNF3YpjE+4VX6c2s5IfU5wuezhltan6ikEL5xpr+9\n"
"S7XDdLiowDiJIsVWI7CsnfkuqNEeTMMYAWvr7v6jK9MszS7hARV8x8wmz75e0J1w\n"
"EIeeHoECgYEAz/OfqenR1g4IZ6bx3bH0JR0wfVuz/WxlJyIlygWKLFp6O+JMBqAr\n"
"boCYh56OdGdKqC8bsUz1YOUJIFB9Hld/Ek4x91CgQfG0GMW745bX+bYTaMcxC028\n"
"uQ8HyY4wKidlLwP4OyNzRu6+jQUYD1o4F4W5p5MvrYpBVsHGcOW6LB8CgYEAzJHh\n"
"nIFl5ZilVmdQk4IGtq5wyur5YxxiVYgZN5pu5pV6RBJGWPW8PpyETGzfDSTurr2E\n"
"bRZjCmHNrANawvxubz1MrMsQtFflga6Dk6z3cgWttvqTpKftc6JaQZm6J/j4FNhb\n"
"Firq/er42KTQn17qSTy1Rmx8h8gfVtwde9pBP/ECgYAgTTOL7SoPGfbsQt5nFXFb\n"
"IoGZt1jqEKBVx9Qe4WVpJlS+YJR2owKcE5Wd2189bZ4HKeofTFkBSBJ21onY4LZ4\n"
"XXUdeYSrOaREBNL+szuxKjsxl19i5hxekpsfD9sjcSMifndcmFEbv5IRzazDFbyI\n"
"ytjf0tkhF8y2cTsl5klY+QKBgQCZMOFHZuscbRvbQVDotWU7phPXEPEiVB4bOOjR\n"
"Z7eGuwctNJYzhgmqxWCZLqo12KNYfZlOa4ZNPwJavsRX4dr9OGVTvFlNA8ldPIdR\n"
"riQ64Z+9t5Wapxo9DqYSHAF4b4uvaHmFdyw0a6Jf8cQ8pEJEB88j5pe2wpkdXYZC\n"
"uiVEsQKBgGW5R9djMC3PdvRmSeQ+VGKbrdYrQ25xzq/8CLVjEhJUqHj2nN1Tak/p\n"
"/lGbF8E2Z7aibjFck8HdzJXOCfW0IgtIijQBIavWHE3y1Xo/2O3I4vI0rNaskqpM\n"
"F7GfrYXd/fjfoXnPfqfnDxEJ4F2Rmw7I2kZ4sOfgCuvxPaad3Rj1\n"
"-----END RSA PRIVATE KEY-----\n";
