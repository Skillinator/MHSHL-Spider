# MHSHL-Spider
Backend web spider to scrape data from the Midwest High School Hockey League Pointstreak pages. This is the server-side component that handles processing to ease things up for the mobile clients for my MHSHL Android app. With some modifications it could potentially be applied to other leagues hosted on the Pointstreak service. 

# Installation
Our only required library is libcurl. We also require a MySQL installation. Add credentials for the database in databasePasswords.h.example and rename the file to databasePasswords.h before compilation.

