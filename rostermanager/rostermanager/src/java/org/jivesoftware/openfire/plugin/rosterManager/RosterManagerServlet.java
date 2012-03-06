/**
 * $RCSfile$
 * $Revision: 1710 $
 * $Date: 2005-07-26 11:56:14 -0700 (Tue, 26 Jul 2005) $
 *
 * Copyright (C) 2004-2008 Jive Software. All rights reserved.
 *
 * This software is published under the terms of the GNU Public License (GPL),
 * a copy of which is included in this distribution, or a commercial license
 * agreement with Jive.
 */

package org.jivesoftware.openfire.plugin.rosterManager;

import org.jivesoftware.openfire.XMPPServer;
import org.jivesoftware.openfire.user.UserNotFoundException;
import org.jivesoftware.openfire.user.UserAlreadyExistsException;
import org.jivesoftware.util.Log;
import org.jivesoftware.openfire.plugin.RosterManagerPlugin;
import org.jivesoftware.admin.AuthCheckFilter;


import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.PrintWriter;


public class RosterManagerServlet extends HttpServlet {

    private RosterManagerPlugin plugin;


    public void init(ServletConfig servletConfig) throws ServletException {
        super.init(servletConfig);
        plugin = (RosterManagerPlugin) XMPPServer.getInstance().getPluginManager().getPlugin("rostermanager");
 
        // Exclude this servlet from requiring the user to login
        AuthCheckFilter.addExclude("rosterManager/rostermanager");

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException
    {

        // Printwriter for writing out responses to browser
        PrintWriter out = response.getWriter();

        if (!plugin.getAllowedIPs().isEmpty()) {
            // Get client's IP address
            String ipAddress = request.getHeader("x-forwarded-for");
            if (ipAddress == null) {
                ipAddress = request.getHeader("X_FORWARDED_FOR");
                if (ipAddress == null) {
                    ipAddress = request.getHeader("X-Forward-For");
                    if (ipAddress == null) {
                        ipAddress = request.getRemoteAddr();
                    }
                }
            }
            if (!plugin.getAllowedIPs().contains(ipAddress)) {
                Log.warn("User service rejected service to IP address: " + ipAddress);
                replyError("RequestNotAuthorised",response, out);
                return;
            }
        }

        String username = request.getParameter("username");
        String jid = request.getParameter("jid");
        String sub = request.getParameter("sub");
        String secret = request.getParameter("secret");
        String type = request.getParameter("type");

       
       // Check that our plugin is enabled.
        if (!plugin.isEnabled()) {
            Log.warn("User service plugin is disabled: " + request.getQueryString());
            replyError("RosterUserDisabled",response, out);
            return;
        }
       
        // Check this request is authorised
        if (secret == null || !secret.equals(plugin.getSecret())){
            Log.warn("An unauthorised user service request was received: " + request.getQueryString());
            replyError("RequestNotAuthorised",response, out);
            return;
         }
        // Check the request type and process accordingly
        try {
        	if(type.equalsIgnoreCase("modify"))
            	plugin.modifyRoster(username,jid,sub);
            else if(type.equalsIgnoreCase("create"))
            	plugin.createRoster(username,jid);
            else if(type.equalsIgnoreCase("remove"))
            	plugin.removeRoster(username,jid);
            replyMessage(response, out);
        }
        catch (UserNotFoundException e) {
            replyError("UserNotFoundException",response, out);
        }
        catch (IllegalArgumentException e) {
            
            replyError("IllegalArgumentException",response, out);
        }
        catch (Exception e) {
            replyError(e.toString(),response, out);
        }
    }

    private void replyMessage(HttpServletResponse response, PrintWriter out){
        response.setContentType("text");        
        out.println("{\"error\":false}");
        out.flush();
    }

    private void replyError(String error,HttpServletResponse response, PrintWriter out){
        response.setContentType("text");        
        out.println("{\"error\":true, \"message\":\""+error+"\"}");
        out.flush();
    }
    
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        doGet(request, response);
    }

    public void destroy() {
        super.destroy();
        // Release the excluded URL
        AuthCheckFilter.removeExclude("rosterManager/rostermanager");
    }
}
