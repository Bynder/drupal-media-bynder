IMPORTANT DISCLAIMER
====================
This module has now been deprecated and is no longer supported or maintained by Bynder. The new version of the module gor both Drupal 7 and 8 has been released and can be found in the [Bynder Drupal project page](https://www.drupal.org/project/bynder).

Please refer to the project page for the latest source code, documentation and to report any issues/requests.



DEPRECATED - drupal-media-bynder
====================

This configuration guide is based on the standard drupal installation, the current version of drupal is 7.x. Any issues can be reported to [github] (https://github.com/Bynder/drupal-media-bynder/issues).

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
Versions matter!

*   [Media 2.x] (https://drupal.org/project/media)
*   [OAuth 1.0 3.x] (https://drupal.org/project/oauth)
*   [File Entity 2.x] (https://www.drupal.org/project/file_entity)
*   [WYSIWYG 2.x] (https://www.drupal.org/project/wysiwyg)
*   [CTools 1.x] (https://www.drupal.org/project/ctools)
*   [Views 3.x] (https://www.drupal.org/project/views)
*   [CKEditor 3.x] (http://ckeditor.com/download)

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
<h3>Add media from Bynder to Drupal Library</h3>
In order to include media from Bynder into Drupal, go to Content and click the link Media. Here you can click on "Add media from Bynder".
Enter a search term and/or click any of the provided facets.
Select an image by clicking on it.
A confirmation "Successfully added media to Library" (in the right upper corner) will be shown and the media file will be available for usage in the Library.

<h3>Add media from Bynder direct to content</h3>
Make sure that under Home » Administration » Configuration » Content authoring » Wysiwyg profiles for the text format of your choice (or multiple) you press 'edit' and under the 'buttons and plugins tab' at least 'Media browser' is enabled.

Now if you go to Home » Add content and add new content you will see the 'Add media' button in your WYSIWYG view. Once pressed it will take you to the Bynder module as well and you can add a Bynder image to your content, skipping adding it to the library.

<h3>Add media from Bynder as a default field on your content with specific size</h3>
You can use this function to (always) add an image to a certain content type. A few examples would be: Every article as in a blog can have a picture of the author. Or a standard page always has to have a banner.

Creates a call to the public Bynder function in /additional/, make sure this is properly configured or ask the Bynder support team.
<ol>
	<li>Add a Style in Home » Administration » Configuration » Media.</li>
	<li>Add the 'Bynder custom resolution' effect and select a width and height smaller than the source image as it does not support upscaling. Make sure this is the only effect applied.</li>
	<li>Go to Home » Administration » Structure » Content types. Click 'manage fields' for the content type of your choice.</li>
	<li>Add a new field. Make sure 'field type' is set to 'Image' and 'widget' to 'media browser'.</li>
	<li>Click 'Save'. On the next page click 'Save field settings' again.</li>
	<li>Change 'Enabled browser plugins' to 'Bynder' only.</li>
	<li>Change 'Allowed URI schemes' to 'Bynder media' only.</li>
	<li>lick 'Save settings' once again.</li>
	<li>Go to Home » Administration » Structure » Content types.  Click 'manage display' for the content type you choose in step 3.</li>
	<li>Set format from '<Hidden>' to 'Image'.</li>
	<li>For your field (named in step 4) press the 'gear icon'.</li>
	<li>Change the 'Image style' to your image style, which you created in step step 1 & 2. If you look at the upper right corner, you will see you just edited this for the 'Default' view mode. You will need to repeat this step for the 'Full content' view mode as well.</li>
	<li>Now you can add new content. Go to Home » Add content. Add here new content of the type specified in step step 3.</li>
	<li>Scroll down and select an image by clicking the 'Browse' button under your added field name.
	<li>Now there should be an image provided by Bynder, with the size specified in step 2, on the content page.</li>
</ol>
