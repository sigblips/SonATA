################################################################################
#
# File:    func.js
# Project: OpenSonATA
# Authors: The OpenSonATA code is the result of many programmers
#          over many years
#
# Copyright 2011 The SETI Institute
#
# OpenSonATA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSonATA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
# 
# Implementers of this code are requested to include the caption
# "Licensed through SETI" with a link to setiQuest.org.
# 
# For alternate licensing arrangements, please contact
# The SETI Institute at www.seti.org or setiquest.org. 
#
################################################################################

function inverse($f) {
    for ($i = 0; $i < $f.elements.length; $i++) {
        if ($f.elements[$i].type == "checkbox") {
            $f.elements[$i].checked = !$f.elements[$i].checked;
        }
    }
    return false;
}

function checkAll($f) {
    for ($i = 0; $i < $f.elements.length; $i++) {
        if ($f.elements[$i].type == "checkbox") {
            $f.elements[$i].checked = true;
        }
    }
    return false;
}

/**
 * Requires prototype.js (http://prototype.conio.net/)
 */
Ajax.ImgUpdater = Class.create();
Ajax.ImgUpdater.prototype = {
    initialize: function(imgID, timeout, newSrc) {
        this.img = document.getElementById(imgID);
        if (newSrc) {
            this.src = newSrc;
        } else {
            this.src = this.img.src;
        }
        this.timeout = timeout;
        this.start();
    },

    start: function() {
        var now = new Date();
        this.img.src = this.src + '&t=' + now.getTime();
        this.timer = setTimeout(this.start.bind(this), this.timeout * 1000);
    },

    stop: function() {
        if (this.timer) clearTimeout(this.timer);
    }
}

function togglePanel(container, remember_url) {
    if (Element.getStyle(container, "display") == 'none') {
        if (remember_url) {
            new Ajax.Request(remember_url, {method:'get',asynchronous:true, parameters: 'state=on'});
        }
        if (document.getElementById('invisible_' + container)) {
            Element.hide('invisible_' + container);
        }
        if (document.getElementById('visible_' + container)) {
            Element.show('visible_' + container);
        }

        Effect.Grow(container);
    } else {
        if (remember_url) {
            new Ajax.Request(remember_url, {method:'get',asynchronous:true, parameters: 'state=off'});
        }
        if (document.getElementById('visible_' + container)) {
            Element.hide('visible_' + container);
        }
        if (document.getElementById('invisible_' + container)) {
            Element.show('invisible_' + container);
        }
        Effect.Shrink(container);
    }
    return false;
}

function scaleImage(v, min, max) {
    var images = document.getElementsByClassName('scale-image');
    w = (max - min) * v + min;
    for (i = 0; i < images.length; i++) {
        images[i].style.width = w + 'px';
    }
}

function toggleAndReloadPanel(container, url) {
    if (Element.getStyle(container, "display") == 'none') {
        new Ajax.Updater(container, url);
        Effect.BlindDown(container);
    } else {
        Effect.Shrink(container);
    }
}

function getWindowHeight() {
    var myHeight = 0;
    if (typeof( window.innerHeight ) == 'number') {
        //Non-IE
        myHeight = window.innerHeight;
    } else if (document.documentElement && document.documentElement.clientHeight) {
        //IE 6+ in 'standards compliant mode'
        myHeight = document.documentElement.clientHeight;
    } else if (document.body && document.body.clientHeight) {
        //IE 4 compatible
        myHeight = document.body.clientHeight;
    }
    return myHeight;
}

function getWindowWidth() {
    var myWidth = 0;
    if (typeof( window.innerWidth ) == 'number') {
        //Non-IE
        myWidth = window.innerWidth;
    } else if (document.documentElement && document.documentElement.clientWidth) {
        //IE 6+ in 'standards compliant mode'
        myWidth = document.documentElement.clientWidth;
    } else if (document.body && document.body.clientWidth) {
        //IE 4 compatible
        myWidth = document.body.clientWidth;
    }
    return myWidth;
}

var helpTimerID;

function setupHelpToggle(url) {
    rules = {
        'li#abbreviations': function(element) {
            element.onclick = function() {
                container='help';
                if (Element.getStyle(container, "display") == 'none') {
                    new Ajax.Updater(container, url);
                }
                Effect.toggle(container, 'appear');
                if (helpTimerID) clearTimeout(helpTimerID)
                helpTimerID = setTimeout('Effect.Fade("'+container+'")', 15000);
                return false;
            }
        }
    }
    Behaviour.register(rules);

}