drupal-media-bynder
====================

This configuration guide is based on the standard drupal installation, the current version of drupal is 7.25. Any issues can be reported to [github] (https://github.com/LabelA/drupal-media-bynder/issues).

For more information please visit [www.getbynder.com](http://www.getbynder.com) or contact us at [info@getbynder.com](mailto:info@getbynder.com)

Server Requirements
-----------------------------------
Curl must be installed and enabled to run the Bynder plugin


Install the media_bynder module
-----------------------------------
Download the latest release and copy the bynder module to "sites/all/modules" and rename the folder to "media_bynder" if necessary or rename the zip file to "media_bynder" before uploading.


Required Modules
---------------------
Upload the following required modules into the folder "sites/all/modules", some modules require additional modules to operate!
*   [Media 2.x] (https://drupal.org/project/media)
*   [OAuth 3.x] (https://drupal.org/project/oauth)

Go to the "Modules" panel in your admin view and enable the listed modules.


Bynder Configuration Settings
---------------------
<h3>Bynder API</h3>
*Bynder url:* The url to be used by the module to communicate with Bynder.  
*Bynder oauth consumer:* The oauth consumer to be used by the module to communicate with Bynder.  
*Bynder oauth consumer secret:* The oauth consumer secret to be used by the module to communicate with Bynder.  
*Bynder oauth token:* The oauth token to be used by the module to communicate with Bynder.  
*Bynder oauth token secret:* The oauth token secret to be used by the module to communicate with Bynder.

These settings require a Bynder account, for more information please visit [www.getbynder.com](http://www.getbynder.com)


Usage of the module
---------------------
In order to include media from Bynder into Drupal, go to Content and click the link Media. Here you can click on "Add media from Bynder".
Enter a search term and/or click any of the provided facets.
Select an image by clicking on it.
A confirmation "Successfully added media to Library" will be shown and the media file will be available for usage in the Library.

