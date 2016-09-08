
#ifndef __CDPI_PROTOCOLS_INCLUDE_FILE__
#define __CDPI_PROTOCOLS_INCLUDE_FILE__

#include "cdpi_main.h"

/* TCP/UDP protocols */
u_int cdpi_search_tcp_or_udp_raw(struct cdpi_detection_module_struct *cdpi_struct,
				 u_int8_t protocol,
				 u_int32_t saddr, u_int32_t daddr,
				 u_int16_t sport, u_int16_t dport);

void cdpi_search_tcp_or_udp(struct cdpi_detection_module_struct *cdpi_struct, struct cdpi_flow_struct *flow);

/* Applications and other protocols. */
void cdpi_search_bittorrent(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_edonkey(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_fasttrack_tcp(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_gnutella(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_winmx_tcp(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_directconnect(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_applejuice_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_i23v5( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_socrates( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_soulseek_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_msn( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_yahoo( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_oscar( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_jabber_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_irc_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_sip( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_direct_download_link_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mail_pop_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mail_imap_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mail_smtp_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_http_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
//void cdpi_http_subprotocol_conf( u_int16_t thread_id, char *attr, char *value, int protocol_id);
void cdpi_search_ftp_control( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ftp_data( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_usenet_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_dns( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_rtsp_tcp_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_filetopia_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_vmware( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_imesh_tcp_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ssl_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mms_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_icecast_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_shoutcast_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
//void cdpi_search_veohtv_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_openft_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_stun( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_tvants_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_sopcast( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_tvuplayer( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ppstream( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_pplive( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_iax( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mgcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_zattoo(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_qq( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_feidian( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ssh_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ayiya( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_thunder(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_activesync( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_in_non_tcp_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_vnc_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_dhcp_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_steam( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_halflife2( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_xbox( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_smb_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_telnet_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ntp_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_nfs( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_rtp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ssdp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_worldofwarcraft( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_postgres_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mysql_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_bgp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_quake( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_battlefield( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_secondlife( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_pcanywhere( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_rdp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_snmp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_kontiki( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_syslog(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_tds_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_netbios( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mdns( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ipp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ldap( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_warcraft3( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_kerberos( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_xdmcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_tftp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_mssql( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_pptp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_stealthnet( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_dhcpv6_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
//void cdpi_search_meebo( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_afp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_aimini( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_florensia( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_maplestory( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_dofus( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_world_of_kung_fu( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_fiesta( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_crossfire_tcp_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_guildwars_tcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_armagetron_udp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_dropbox( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_skype( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_citrix( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_dcerpc( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_netflow( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_sflow( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_radius( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_wsus( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_teamview( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_lotus_notes( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_gtp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_spotify( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_h323( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_openvpn( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_noe( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_ciscovpn( u_int16_t thread_id, struct cdpi_flow_struct *flow);
//void cdpi_search_viber( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_teamspeak( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_corba( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_collectd( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_oracle(u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_rsync( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_rtcp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_skinny( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_tor( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_whois_das( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_socks5( struct cdpi_detection_module_struct *cdpi_struct, struct cdpi_flow_struct *flow);
void cdpi_search_socks4( struct cdpi_detection_module_struct *cdpi_struct, struct cdpi_flow_struct *flow);
void cdpi_search_rtmp( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_pando( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_megaco( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_redis( u_int16_t thread_id, struct cdpi_flow_struct *flow);
void cdpi_search_zmq( u_int16_t thread_id, struct cdpi_flow_struct *flow);

#endif /* __CDPI_PROTOCOLS_INCLUDE_FILE__ */
