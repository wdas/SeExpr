/*
 * $Revision$
 * $Date$
 *
 * Copyright (C) 1999-2012 Jive Software. All rights reserved.
 *
 * This software is the proprietary information of Jive Software. Use is subject to license terms.
 */
if (typeof(jive) != "undefined") {

    /**
     * defines a simple Macro interface to mimic the RenderMacro class on the server
     */
    jive.rte.plugin.videomacro = function (shortname, url, macrotag, settingsHuh, displayHuh, paramSets, params,
                                           enabled, button)
    {
        var that = this;

        params = [
            {}
        ];

        /**
         * gets the unique name for this macro
         * i.e. "code" or "youtube"
         */
        this.getName = function () {
            return shortname;
        };

        /**
         * gets the optional url for this macro
         */
        this.getUrl = function () {
            return url;
        };

        /**
         * returns true if it should be a button or not
         */
        this.isButton = function () {
            return button;
        };

        this.isEnabled = function () {
            return enabled;
        };

        this.isShowSettings = function () {
            return settingsHuh;
        };

        /**
         * Display in RTE Insert List?
         */
        this.isShowInMacroList = function () {
            return displayHuh;
        };

        /**
         * returns true if this macro accepts
         * raw text input, like a code macro,
         * or false if it doesn't, like
         * a youtube macro
         */
        this.getMacroType = function () {
            return macrotag;
        };

        /**
         * returns an array of allowed parameters
         */
        this.getAllowedParameters = function () {
            return params;
        };

        this.usesCustomBackground = function () {
            return true;
        };

        /**
         * update the position of $obj to properly display behind ele's content
         * in the RTE
         * @param rte the jive RTE object
         * @param ele the DOM element /inside/ the RTE proper. this should
         * be clear so show through to $obj behind it
         * @param $ele the DOM element /behind/ the RTE that shows rich content
         */
        this.refreshPosition = function (rte, ele, $ele) {
//

            that.updatePreviewContentPosition(ele, $ele); //todo not sure we need this extra call
        };

        this.updatePreviewContentPosition = function (ele, $ele) {
            // get offset inside the RTE
            var t = jive.ext.x.xPageY(ele);
            var l = jive.ext.x.xPageX(ele);

            if ($ele) {

                if ($ele.css("top") != t) {
                    $ele.css("top", t);
                }

                if ($ele.css("left") != l) {
                    $ele.css("left", l);
                }
            }
        };

        /**
         * update the element's display w/ the latest
         * parameter value.
         */
        this.refresh = function (rte, ele) {
            var $ele = rte.getHiddenElementFor($j(ele).attr("_jivemacro_uid"));

            if ($ele.data("init") == null) {
                // because this is called *immediately* after the insert macro
                // the browser hasn't actually set the width/height correctly
                // so we need to wait for just a bit before refreshing the graph
                setTimeout(function (rte, ele, $ele) {
                    return function () {
                        that.displayVideo(rte, ele, $ele);
                        that.updatePreviewContentPosition(ele, $ele);
                    }
                }(rte, ele, $ele), 328);
            }
            else {
                that.displayVideo(rte, ele, $ele);
            }
        };

        this.displayVideo = function (rte, ele, $ele) {
            var uid = ele.getAttribute("_jivemacro_uid");

            // fix the legacy elements for id to videoid conversion since ids are stripped
            var $e = $j(ele);
            var _id = $e.attr('_id');
            var _videoid = $e.attr('_videoid');
            if (_id && !_videoid) {
                $e.attr('_videoid', _id);
            }

            var previewImgSrc = ele.getAttribute("_imageURL");
            if (previewImgSrc) {
                $j(ele).width(520)
                    .height(328)
                    .css("background", "transparent");
                $ele.width(520)
                    .height(328)
                    .html("<img src='" + previewImgSrc + "/520'/>");
            }
        };


        this.retrieveInsertionUUID = function ($ele) {
            return $j($ele, '#insertionUUID').attr("id");
        };

        this.getEditorWindowHandle = function () {
            var w;

            if (typeof(window.parent.editor) == "undefined") {
                w = window.parent.tinyMCE.activeEditor.getWin().parent;
            }
            else {
                w = window.parent;
            }
            return w;
        };

        this.doSetResizedDimensions = function (ele, $ele, width, height, superRichContentMaxWidth) {
            $ele.width(superRichContentMaxWidth - 500 > width ? superRichContentMaxWidth - 500 : width);
            $ele.height(height);
            $j(ele).width(superRichContentMaxWidth - 500 > width ? superRichContentMaxWidth - 500 : width);
            $j(ele).height(height);
            $j(ele).attr('src', "/images/tiny_mce3/plugins/alignment/img/spacer.gif");
        }
    }


    // bind to app artifacts in the RTE (RTE itself need not necessarily be visible). So any rendered content.
    if (jive.rte && jive.rte.renderedContent) {
        jive.rte.renderedContent.addListener("renderedContentWithSelector", function ($renderedContent, opts) {

            $renderedContent.find('div.jive-content-video[data-video-id]').each(function (index, element) {

                var $element = $j(element);

                if ($element.attr('data-video-id')) {

                    // TODO move it to videos's specific source.js maybe so it's a real MVC and subclasses RestService
                    // get the object data and add the auth token
                    var objectType = $element.attr('data-object-type');
                    var objectID = $element.attr('data-object-id');
                    var videoID = $element.attr('data-video-id');
                    var divID = $element.attr("id");
                    getVideoDataAndRender(divID, videoID, objectType, objectID);
                }


            });
        });
    }

}