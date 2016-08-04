/*
 * $Revision$
 * $Date$
 *
 * Copyright (C) 1999-2012 Jive Software. All rights reserved.
 *
 * This software is the proprietary information of Jive Software. Use is subject to license terms.
 *
 */

/**
 * @depends template=jive.videos.common.noFlash
 *
 * @depends path=/resources/scripts/jive/i18n.js
 */
function renderVideoWithFlash(divID, video) {
    var videoID = video.externalID ? "" + video.externalID : '';                                     
    var movie = video.playerBaseURL + '/plugins/player.swf?v=' + videoID + '&auth_token=' + video.authtoken + '&p=' + video.playerName;
    var base = video.playerBaseURL;
    var autoplay = video.autoplay; 
    var src = video.playerBaseURL + '/plugins/player.swf?p=' + video.playerName + '&auth_token=' + video.authtoken;
    var watermark = video.watermarkURL ? video.watermarkURL : '';
    var width = video.width? video.width : 520;
    var height = video.height? video.height : 328;
    
    var $div, $parent_div;
    var lightboxme = false;
    
    if (divID) {
        $div = $j('#' + divID);
    }    
    // if no div then should create one and lightbox me
    else {
        $div = $j('<div id="as-video-container" class="media { type:\'swf\' }"></div>');
        $parent_div = $j('<div id="lb_image_wrapper" class="jive-modal"><a class="j-modal-close-top close j-icon-close" href="#"><span class="j-close-icon j-ui-elem" role="img"></span></a></div>');
        $parent_div.append($div);
        $j('body').append($parent_div);                      
        lightboxme = true;
    }       
    
    if ($div.find('object').length > 0) {
        return;
    }

    require(['jquery.media'], function() {
        var div = $div.media({
            width:     width,
            height:    height,
            flashVersion: '9.0.115.0',
            autoplay: '\'' + autoplay + '\'' ,
            src:  src,
            attrs:     { id: 'embedded_player', tabindex: '0' },  // object/embed attrs
            params:    {
                    allowscriptaccess: 'always',
                    allowfullscreen: 'true',
                    movie: movie,
                    base: base,
                    bgColor: '#000000',
                    quality: 'high',
                    wmode: 'opaque'
                }, // object params/embed attrs
            flashvars: {
                v:'0',
                config: '{config:{autoplay:' + autoplay + '}}' ,
                l:'[{video_id:\'' + videoID +'\',logo_file:\''+ watermark + '\'}]'
                },
            caption: false // supress caption text
        });

        if (lightboxme) {

            $parent_div.lightbox_me({
                destroyOnClose: true,
                onClose: function() {
                    $j('#lb_image_wrapper').remove();
                },
                closeSelector: ".jive-modal-close-top, .close",
                onLoad: function() {

                    if ($j('#embedded_player').length == 0) {
                        var player = $j('object').first();
                        if (player) {
                            player.attr('id', 'embedded_player');
                            player.attr('tabindex', '0');
                        }

                    }
                    $j('#embedded_player').get(0).focus();
                }
            });
        }

        if (div.find('object').length == 0) {
            $div.after(jive.videos.common.noFlash());
        }
    });
}

/**
 * ObjectID and ObjectType are for the object the video is embedded into
 * for permission checking purposes but
 * in case of the video rendering itself, it's the same as video
 *
 * @param divID
 * @param videoID
 * @param objectType
 * @param objectID
 */
function getVideoDataAndRender(divID,videoID,objectType,objectID) {

    var url = jive.rest.url("/videos");

    $j.ajax({
        type: "GET",
        url: url + "/" + videoID + "?objectType="+objectType+"&objectID="+objectID,
        success: function(video) {
            if (video.authtoken) {
                renderVideo(divID, video)
            }

        }
    });
}

/**
 *
 * @depends i18nKeys=video.encoding.text
 *
 * @param divID
 * @param video
 */
function renderVideo(divID, video) {

    if (!video.doneTranscoding) {

        var $warningDiv = $j(
            '<div class="jive-warn-box" aria-live="polite" aria-atomic="true"><div>' +
                '<span class="jive-icon-med jive-icon-warn"></span>'
                + jive.i18n.getMsg("video.encoding.text") +
                '</div>' +
                '</div>')

        $j('#' + divID).prepend($warningDiv);
        return;
    }

    if(isFlashEnabled() && !video.mobileEnabled) {
        renderVideoWithFlash(divID, video);
        return;
    }

    var videoID = video.externalID ? "" + video.externalID : '';
    var width = (video.width? video.width : 520);
    var height = (video.height? video.height : 328);

    var $div, $parent_div;
    var lightboxme = false;

    if (divID) {
        $div = $j('#' + divID);
    }
    else {
        $div = $j('<div id="as-video-container" class="media { type:\'swf\' }"></div>');
        $parent_div = $j('<div id="lb_image_wrapper" class="jive-modal"><a class="j-modal-close-top close j-icon-close" href="#"><span class="j-close-icon j-ui-elem" role="img"></span></a></div>');
        $parent_div.append($div);
        $j('body').append($parent_div);
        lightboxme = true;
    }

    // Twistage JS won't work because header_javascript.ftl blocks document.write calls
    // so use iFrame to host the video and still use their code
    var $vidContents = $j('<iframe width="'+ (width + 25) + '" height="'+ (height +25) +'" id="iframe_'+ videoID +'">');

    // Swap out video and placeholder image
    $div.find('img').replaceWith($vidContents);

    if($vidContents[0].contentWindow == null) {
        $div.replaceWith($vidContents);
    }

    var doc = ($vidContents[0].contentWindow || $vidContents[0].contentDocument);
    if(doc.document) doc = doc.document;

    var script = doc.createElement('script');
    script.type = 'text/javascript';
    script.src = video.playerBaseURL + '/api/script';
    var head = doc.getElementsByTagName('head')[0];

    script.onload = script.onreadystatechange = function() {
        if (!this.readyState ||this.readyState == "loaded" || this.readyState == "complete" ) {
            var s = doc.createElement('script');
            s.type = 'text/javascript';
            s.text = 'viewNode(\''+ videoID + '\',{ server_detection: true, width: 520, height: 328, player_profile:\''+video.playerName+'\', auth_token:\''+video.authtoken +'\'});'
            head.insertBefore(s, head.firstChild);
        }
    };

    head.insertBefore(script, head.firstChild);

    if (lightboxme) {

        $parent_div.lightbox_me({
            destroyOnClose: true,
            onClose: function() {
                $j('#lb_image_wrapper').remove();
            },
            closeSelector: ".jive-modal-close-top, .close"
        });
    }

}

//checks if flash is installed/enabled on the browser
function isFlashEnabled()
{
    var hasFlash = false;
    try
    {
        var fo = new ActiveXObject('ShockwaveFlash.ShockwaveFlash');
        if(fo) hasFlash = true;
    }
    catch(e)
    {
        if(navigator.mimeTypes ["application/x-shockwave-flash"] != undefined) hasFlash = true;
    }
    return hasFlash;
}
                  

