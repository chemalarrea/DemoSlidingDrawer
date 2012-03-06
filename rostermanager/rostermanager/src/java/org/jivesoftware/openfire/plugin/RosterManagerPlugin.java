/**
 * $Revision: 1722 $
 * $Date: 2005-07-28 15:19:16 -0700 (Thu, 28 Jul 2005) $
 *
 * Copyright (C) 2005-2008 Jive Software. All rights reserved.
 *
 * This software is published under the terms of the GNU Public License (GPL),
 * a copy of which is included in this distribution, or a commercial license
 * agreement with Jive.
 */

package org.jivesoftware.openfire.plugin;

import org.jivesoftware.openfire.XMPPServer;
import org.jivesoftware.openfire.container.Plugin;
import org.jivesoftware.openfire.container.PluginManager;
import org.jivesoftware.openfire.group.Group;
import org.jivesoftware.openfire.group.GroupManager;
import org.jivesoftware.openfire.group.GroupNotFoundException;
import org.jivesoftware.openfire.user.User;
import org.jivesoftware.openfire.user.UserAlreadyExistsException;
import org.jivesoftware.openfire.user.UserManager;
import org.jivesoftware.openfire.user.UserNotFoundException;
import org.jivesoftware.util.JiveGlobals;
import org.jivesoftware.util.StringUtils;
import org.jivesoftware.util.PropertyEventListener;
import org.jivesoftware.util.PropertyEventDispatcher;
import org.xmpp.packet.JID;
import org.jivesoftware.openfire.roster.RosterItem;
import org.jivesoftware.openfire.SharedGroupException;


import java.io.File;
import java.util.*;

/**
 * Plugin that allows the administration of users via HTTP requests.
 *
 * @author Justin Hunt
 */
public class RosterManagerPlugin implements Plugin, PropertyEventListener {
    private UserManager userManager;
    private XMPPServer server;

    private String secret;
    private boolean enabled;
    private Collection<String> allowedIPs;

    public void initializePlugin(PluginManager manager, File pluginDirectory) {
        server = XMPPServer.getInstance();
        userManager = server.getUserManager();

        secret = JiveGlobals.getProperty("plugin.rostermanager.secret", "");
        // If no secret key has been assigned to the user service yet, assign a random one.
        if (secret.equals("")){
            secret = StringUtils.randomString(8);
            setSecret(secret);
        }
        
        // See if the service is enabled or not.
        enabled = JiveGlobals.getBooleanProperty("plugin.rostermanager.enabled", false);

        // Get the list of IP addresses that can use this service. An empty list means that this filter is disabled.
        allowedIPs = StringUtils.stringToCollection(JiveGlobals.getProperty("plugin.rostermanager.allowedIPs", ""));

        // Listen to system property events
        PropertyEventDispatcher.addListener(this);
    }

    public void destroyPlugin() {
        userManager = null;
        // Stop listening to system property events
        PropertyEventDispatcher.removeListener(this);
    }

	public void modifyRoster(String nombre, String jid,String sub) throws UserNotFoundException{

		User usuario = getUser(nombre);
		if(sub.equalsIgnoreCase("none")){
			usuario.getRoster().getRosterItem(new JID(jid)).setSubStatus(RosterItem.SUB_NONE);
		}else if(sub.equalsIgnoreCase("both")){
			usuario.getRoster().getRosterItem(new JID(jid)).setSubStatus(RosterItem.SUB_BOTH);
		}else if(sub.equalsIgnoreCase("to")){
			usuario.getRoster().getRosterItem(new JID(jid)).setSubStatus(RosterItem.SUB_TO);
		}else if(sub.equalsIgnoreCase("from")){
			usuario.getRoster().getRosterItem(new JID(jid)).setSubStatus(RosterItem.SUB_FROM);
		}else if(sub.equalsIgnoreCase("remove")){
			usuario.getRoster().getRosterItem(new JID(jid)).setSubStatus(RosterItem.SUB_REMOVE);
		}else{
			throw new IllegalArgumentException();
		}
	}
	
	public void createRoster(String nombre, String jid)throws UserAlreadyExistsException,
																	SharedGroupException,
																	UserNotFoundException{
		User usuario = getUser(nombre);
		usuario.getRoster().createRosterItem(new JID(jid),true,true);
		
	}
	
	public void removeRoster(String nombre, String jid) throws SharedGroupException,
																UserNotFoundException{

		User usuario = getUser(nombre);
		usuario.getRoster().deleteRosterItem(new JID(jid),true);
	}
	
    
    /**
     * Returns the the requested user or <tt>null</tt> if there are any
     * problems that don't throw an error.
     *
     * @param username the username of the local user to retrieve.
     * @return the requested user.
     * @throws UserNotFoundException if the requested user
     *         does not exist in the local server.
     */
    private User getUser(String username) throws UserNotFoundException {
        JID targetJID = server.createJID(username, null);
        // Check that the sender is not requesting information of a remote server entity
        if (targetJID.getNode() == null) {
            // Sender is requesting presence information of an anonymous user
            throw new UserNotFoundException("Username is null");
        }
        return userManager.getUser(targetJID.getNode());
    }
    
    /**
     * Returns the secret key that only valid requests should know.
     *
     * @return the secret key.
     */
    public String getSecret() {
        return secret;
    }

    /**
     * Sets the secret key that grants permission to use the rostermanager.
     *
     * @param secret the secret key.
     */
    public void setSecret(String secret) {
        JiveGlobals.setProperty("plugin.rostermanager.secret", secret);
        this.secret = secret;
    }

    public Collection<String> getAllowedIPs() {
        return allowedIPs;
    }

    public void setAllowedIPs(Collection<String> allowedIPs) {
        JiveGlobals.setProperty("plugin.rostermanager.allowedIPs", StringUtils.collectionToString(allowedIPs));
        this.allowedIPs = allowedIPs;
    }

    /**
     * Returns true if the user service is enabled. If not enabled, it will not accept
     * requests to create new accounts.
     *
     * @return true if the user service is enabled.
     */
    public boolean isEnabled() {
        return enabled;
    }

    /**
     * Enables or disables the user service. If not enabled, it will not accept
     * requests to create new accounts.
     *
     * @param enabled true if the user service should be enabled.
     */
    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
        JiveGlobals.setProperty("plugin.rostermanager.enabled",  enabled ? "true" : "false");
    }

    public void propertySet(String property, Map<String, Object> params) {
        if (property.equals("plugin.rostermanager.secret")) {
            this.secret = (String)params.get("value");
        }
        else if (property.equals("plugin.rostermanager.enabled")) {
            this.enabled = Boolean.parseBoolean((String)params.get("value"));
        }
        else if (property.equals("plugin.rostermanager.allowedIPs")) {
            this.allowedIPs = StringUtils.stringToCollection((String)params.get("value"));
        }
    }

    public void propertyDeleted(String property, Map<String, Object> params) {
        if (property.equals("plugin.rostermanager.secret")) {
            this.secret = "";
        }
        else if (property.equals("plugin.rostermanager.enabled")) {
            this.enabled = false;
        }
        else if (property.equals("plugin.rostermanager.allowedIPs")) {
            this.allowedIPs = Collections.emptyList();
        }
    }

    public void xmlPropertySet(String property, Map<String, Object> params) {
        // Do nothing
    }

    public void xmlPropertyDeleted(String property, Map<String, Object> params) {
        // Do nothing
    }
}
