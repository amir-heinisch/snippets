# Dockerfile example.
# Builds a container for web dev with apache, php and mssql drivers.

# Select base image
# Based on Debian 9
FROM php:7.0-apache

# Copy php config into the image.
COPY configs/php.ini /usr/local/etc/php/php.ini

# Copy apache config into the image.
COPY configs/httpd.conf /etc/apache2/httpd.conf

# Change working dir
WORKDIR /var/www/html/

# Open needed ports internaly
EXPOSE 80 443

# Change locale
RUN sed -i 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/g' /etc/locale.gen 
RUN locale-gen

# add https for apt
ARG DEBIAN_FRONTEND=noninteractive 
ARG APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=DontWarn
RUN apt-get update && apt-get install --assume-yes apt-utils gnupg apt-transport-https

# Install microsoft odbc for sql server
RUN curl -s https://packages.microsoft.com/keys/microsoft.asc | apt-key add -
RUN curl -s https://packages.microsoft.com/config/debian/9/prod.list > /etc/apt/sources.list.d/mssql-release.list
RUN apt-get update
RUN ACCEPT_EULA=Y apt-get install -y msodbcsql17

# install unixODBC development headers
RUN apt-get install -y unixodbc-dev

# Install php drivers for mssql server
RUN pecl install sqlsrv
RUN pecl install pdo_sqlsrv
RUN echo extension=pdo_sqlsrv.so >> `php --ini | grep "Scan for additional .ini files" | sed -e "s|.*:\s*||"`/30-pdo_sqlsrv.ini
RUN echo extension=sqlsrv.so >> `php --ini | grep "Scan for additional .ini files" | sed -e "s|.*:\s*||"`/20-sqlsrv.ini

# Create volume
VOLUME /var/www/html

