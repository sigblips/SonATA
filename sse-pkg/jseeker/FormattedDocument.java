package opensonata.dataDisplays;

// ====================================================================
// Filename:   FormattedDocument
// 
// from: http://java.sun.com/docs/books/tutorial/uiswing/components/example-swing/DecimalField.java
// and from: http://java.sun.com/docs/books/tutorial/uiswing/components/example-swing/FormattedDocument.java
//
//
//Copyright © 2008, 2010 Oracle and/or its affiliates. All rights reserved. 
//Use is subject to license terms.
//Redistribution and use in source and binary forms, with or without 
//modification, are permitted provided that the following conditions are met:
//
//Redistributions of source code must retain the above copyright notice, 
//this list of conditions and the following disclaimer.
//Redistributions in binary form must reproduce the above copyright notice, 
//this list of conditions and the following disclaimer in the documentation 
//and/or other materials provided with the distribution.
//Neither the name of Oracle Corporation nor the names of its contributors 
//may be used to endorse or promote products derived from this software 
//without specific prior written permission.
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ""AS IS""
//AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
//ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//POSSIBILITY OF SUCH DAMAGE.
//
//You acknowledge that this software is not designed, licensed or intended 
//for use in the design, construction, operation or maintenance of any nuclear 
//facility.
//
//
// ====================================================================

import javax.swing.*;
import javax.swing.text.*;

import java.text.*;


class FormattedDocument extends PlainDocument {
  private Format format;
  
  public FormattedDocument(Format f) {
    format = f;
  }
  
  public Format getFormat() {
    return format;
  }
  
  public void insertString(int offs, String str, AttributeSet a) 
      throws BadLocationException {
      
      String currentText = getText(0, getLength());
      String beforeOffset = currentText.substring(0, offs);
      String afterOffset = currentText.substring(offs, currentText.length());
      String proposedResult = beforeOffset + str + afterOffset;
	 
      try {
	  format.parseObject(proposedResult);
	  super.insertString(offs, str, a);
      } catch (ParseException e) {
	  // TBD Toolkit.getDefaultToolkit().beep();
	  // TBD System.err.println("insertString: could not parse: "
	  // TBD + proposedResult);
      }
  }
  
  public void remove(int offs, int len) throws BadLocationException {
    String currentText = getText(0, getLength());
    String beforeOffset = currentText.substring(0, offs);
    String afterOffset = currentText.substring(len + offs,
					       currentText.length());
    String proposedResult = beforeOffset + afterOffset;
    
    try {
	if (proposedResult.length() != 0)
	    format.parseObject(proposedResult);
	super.remove(offs, len);
    } catch (ParseException e) {
	// TBD Toolkit.getDefaultToolkit().beep();
	System.err.println("remove: could not parse: " + proposedResult);
    }
  }
}

