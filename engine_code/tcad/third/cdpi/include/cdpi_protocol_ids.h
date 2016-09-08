
#ifndef __CDPI_API_INCLUDE_FILE__

#endif

#ifndef __CDPI_PROTOCOLS_DEFAULT_H__
#define __CDPI_PROTOCOLS_DEFAULT_H__

#define CDPI_DETECTION_SUPPORT_IPV6
#define CDPI_PROTOCOL_HISTORY_SIZE				1

#define CDPI_PROTOCOL_UNKNOWN					0

#define CDPI_PROTOCOL_IP_VRRP 				    73
#define CDPI_PROTOCOL_IP_IPSEC					79
#define CDPI_PROTOCOL_IP_GRE					80
#define CDPI_PROTOCOL_IP_ICMP					81
#define CDPI_PROTOCOL_IP_IGMP					82
#define CDPI_PROTOCOL_IP_EGP					83
#define CDPI_PROTOCOL_IP_SCTP					84
#define CDPI_PROTOCOL_IP_OSPF					85
#define CDPI_PROTOCOL_IP_IP_IN_IP				86
#define CDPI_PROTOCOL_IP_ICMPV6					102

#define CDPI_PROTOCOL_HTTP					7
//#define CDPI_PROTOCOL_HTTP_APPLICATION_VEOHTV 		        60
#define CDPI_PROTOCOL_SSL_NO_CERT			        64
#define CDPI_PROTOCOL_SSL					91
#define CDPI_PROTOCOL_HTTP_APPLICATION_ACTIVESYNC		110
#define CDPI_PROTOCOL_HTTP_CONNECT				130
#define CDPI_PROTOCOL_HTTP_PROXY				131
#define CDPI_PROTOCOL_SOCKS5					172
//#define CDPI_PROTOCOL_SOCKS4					173

#define CDPI_PROTOCOL_FTP_CONTROL				1
#define CDPI_PROTOCOL_MAIL_POP				        2
#define CDPI_PROTOCOL_MAIL_SMTP				        3
#define CDPI_PROTOCOL_MAIL_IMAP			  	        4
#define CDPI_PROTOCOL_DNS              			        5
//#define CDPI_PROTOCOL_IPP					6
#define CDPI_PROTOCOL_MDNS					8
#define CDPI_PROTOCOL_NTP					9
#define CDPI_PROTOCOL_NETBIOS					10
#define CDPI_PROTOCOL_NFS					11
#define CDPI_PROTOCOL_SSDP					12
#define CDPI_PROTOCOL_BGP					13
#define CDPI_PROTOCOL_SNMP					14
#define CDPI_PROTOCOL_XDMCP					15
#define CDPI_PROTOCOL_SMB					16
#define CDPI_PROTOCOL_SYSLOG					17
#define CDPI_PROTOCOL_DHCP					18
#define CDPI_PROTOCOL_POSTGRES				        19
#define CDPI_PROTOCOL_MYSQL					20
//#define CDPI_PROTOCOL_TDS					21
#define CDPI_PROTOCOL_DIRECT_DOWNLOAD_LINK			22
#define CDPI_PROTOCOL_MAIL_POPS				        23
//#define CDPI_PROTOCOL_APPLEJUICE				24
#define CDPI_PROTOCOL_DIRECTCONNECT				25
//#define CDPI_PROTOCOL_SOCRATES				        26
#define CDPI_PROTOCOL_WINMX					27
#define CDPI_PROTOCOL_VMWARE					28
#define CDPI_PROTOCOL_MAIL_SMTPS				29
#define CDPI_PROTOCOL_FILETOPIA				        30
//#define CDPI_PROTOCOL_IMESH					31
//#define CDPI_PROTOCOL_KONTIKI					32
//#define CDPI_PROTOCOL_OPENFT					33
//#define CDPI_PROTOCOL_FASTTRACK				        34
#define CDPI_PROTOCOL_GNUTELLA				        35
#define CDPI_PROTOCOL_EDONKEY					36
#define CDPI_PROTOCOL_BITTORRENT				37
//#define CDPI_PROTOCOL_EPP					38
#define	CDPI_PROTOCOL_XBOX					47
#define	CDPI_PROTOCOL_QQ					48
//#define	CDPI_PROTOCOL_MOVE					49
#define	CDPI_PROTOCOL_RTSP					50
#define CDPI_PROTOCOL_MAIL_IMAPS				51
#define CDPI_PROTOCOL_ICECAST					52
#define CDPI_PROTOCOL_PPLIVE					53
#define CDPI_PROTOCOL_PPSTREAM				        54
//#define CDPI_PROTOCOL_ZATTOO					55
#define CDPI_PROTOCOL_SHOUTCAST				        56
#define CDPI_PROTOCOL_SOPCAST					57
//#define CDPI_PROTOCOL_TVANTS					58
//#define CDPI_PROTOCOL_TVUPLAYER				        59
#define CDPI_PROTOCOL_QQLIVE					61
#define CDPI_PROTOCOL_THUNDER					62
#define CDPI_PROTOCOL_SOULSEEK				        63
#define CDPI_PROTOCOL_IRC					65
#define CDPI_PROTOCOL_AYIYA					66
#define CDPI_PROTOCOL_UNENCRYPED_JABBER			        67
#define CDPI_PROTOCOL_MSN					68
#define CDPI_PROTOCOL_OSCAR					69
#define CDPI_PROTOCOL_WEBYAHOO 				70
#define CDPI_PROTOCOL_BATTLEFIELD				71
#define CDPI_PROTOCOL_QUAKE					72
#define CDPI_PROTOCOL_STEAM					74
#define CDPI_PROTOCOL_HALFLIFE2				        75
#define CDPI_PROTOCOL_WORLDOFWARCRAFT				76
#define CDPI_PROTOCOL_TELNET					77
#define CDPI_PROTOCOL_STUN					78
#define	CDPI_PROTOCOL_RTP					87
#define CDPI_PROTOCOL_RDP					88
#define CDPI_PROTOCOL_VNC					89
#define CDPI_PROTOCOL_PCANYWHERE				90
#define CDPI_PROTOCOL_SSH					92
//#define CDPI_PROTOCOL_USENET					93
//#define CDPI_PROTOCOL_MGCP					94
#define CDPI_PROTOCOL_IAX					95
#define CDPI_PROTOCOL_TFTP					96
#define CDPI_PROTOCOL_AFP					97
//#define CDPI_PROTOCOL_STEALTHNET				98
#define CDPI_PROTOCOL_AIMINI					99
#define CDPI_PROTOCOL_SIP					100
#define CDPI_PROTOCOL_TRUPHONE				        101
#define CDPI_PROTOCOL_DHCPV6					103
//#define CDPI_PROTOCOL_ARMAGETRON				104
#define CDPI_PROTOCOL_CROSSFIRE				        105
//#define CDPI_PROTOCOL_DOFUS					106
//#define CDPI_PROTOCOL_FIESTA					107
//#define CDPI_PROTOCOL_FLORENSIA				        108
#define CDPI_PROTOCOL_GUILDWARS				        109
#define CDPI_PROTOCOL_KERBEROS				        111
#define CDPI_PROTOCOL_LDAP					112
//#define CDPI_PROTOCOL_MAPLESTORY				113
#define CDPI_PROTOCOL_MSSQL					114
#define CDPI_PROTOCOL_PPTP					115
#define CDPI_PROTOCOL_WARCRAFT3				        116
#define CDPI_PROTOCOL_WORLD_OF_KUNG_FU			        117
//#define CDPI_PROTOCOL_MEEBO					118
#define CDPI_PROTOCOL_DROPBOX					121
#define CDPI_PROTOCOL_SKYPE					125
#define CDPI_PROTOCOL_DCERPC					127
#define CDPI_PROTOCOL_NETFLOW					128
#define CDPI_PROTOCOL_SFLOW					129
#define CDPI_PROTOCOL_CITRIX					132
//#define CDPI_PROTOCOL_SKYFILE_PREPAID				136
//#define CDPI_PROTOCOL_SKYFILE_RUDICS				137
//#define CDPI_PROTOCOL_SKYFILE_POSTPAID			        138
#define CDPI_PROTOCOL_CITRIX_ONLINE				139
#define CDPI_PROTOCOL_WEBEX					141
//#define CDPI_PROTOCOL_VIBER					144
#define CDPI_PROTOCOL_RADIUS					146
#define CDPI_PROTOCOL_WINDOWS_UPDATE				147
#define CDPI_PROTOCOL_TEAMVIEWER				148
#define CDPI_PROTOCOL_LOTUS_NOTES				150
#define CDPI_PROTOCOL_SAP					151
#define CDPI_PROTOCOL_GTP					152
#define CDPI_PROTOCOL_UPNP					153
#define CDPI_PROTOCOL_LLMNR					154
#define CDPI_PROTOCOL_REMOTE_SCAN				155
#define CDPI_PROTOCOL_SPOTIFY					156
#define CDPI_PROTOCOL_H323					158
#define CDPI_PROTOCOL_OPENVPN					159
#define CDPI_PROTOCOL_NOE					160
#define CDPI_PROTOCOL_CISCOVPN				        161
#define CDPI_PROTOCOL_TEAMSPEAK				        162
#define CDPI_PROTOCOL_TOR					163
#define CDPI_PROTOCOL_SKINNY					164
#define CDPI_PROTOCOL_RTCP					165
#define CDPI_PROTOCOL_RSYNC					166
#define CDPI_PROTOCOL_ORACLE					167
//#define CDPI_PROTOCOL_CORBA					168
#define CDPI_PROTOCOL_UBUNTUONE			  	        169
#define CDPI_PROTOCOL_WHOIS_DAS				        170
#define CDPI_PROTOCOL_COLLECTD				        171
#define CDPI_PROTOCOL_RTMP					174
#define CDPI_PROTOCOL_FTP_DATA				        175
//#define CDPI_PROTOCOL_ZMQ                                       177
#define CDPI_PROTOCOL_MEGACO    				181
#define CDPI_PROTOCOL_REDIS                                     182
#define CDPI_PROTOCOL_PANDO					183

//#define CDPI_CONTENT_AVI				39
//#define CDPI_CONTENT_FLASH				40
//#define CDPI_CONTENT_OGG				41
//#define	CDPI_CONTENT_MPEG				42
//#define	CDPI_CONTENT_QUICKTIME				43
//#define	CDPI_CONTENT_REALMEDIA				44
#define	CDPI_CONTENT_WINDOWSMEDIA			45
//#define	CDPI_CONTENT_MMS				46
//#define CDPI_CONTENT_WEBM				157

//#define CDPI_SERVICE_FACEBOOK				119
//#define CDPI_SERVICE_TWITTER				120
#define CDPI_SERVICE_GMAIL				122
#define CDPI_SERVICE_GOOGLE_MAPS			123
//#define CDPI_SERVICE_YOUTUBE				124
#define CDPI_SERVICE_GOOGLE				331
//#define CDPI_SERVICE_NETFLIX				133
#define CDPI_SERVICE_LASTFM				134
//#define CDPI_SERVICE_GROOVESHARK			135
#define CDPI_SERVICE_APPLE				140
#define CDPI_SERVICE_WHATSAPP				142
#define CDPI_SERVICE_APPLE_ICLOUD			143
#define CDPI_SERVICE_APPLE_ITUNES			145
#define CDPI_SERVICE_TUENTI				149
#define CDPI_SERVICE_WIKIPEDIA				176
#define CDPI_SERVICE_MSN				CDPI_PROTOCOL_MSN
#define CDPI_SERVICE_AMAZON				178
#define CDPI_SERVICE_EBAY				179
#define CDPI_SERVICE_CNN				180
#define CDPI_SERVICE_DROPBOX				CDPI_PROTOCOL_DROPBOX
#define CDPI_SERVICE_SKYPE				CDPI_PROTOCOL_SKYPE
//#define CDPI_SERVICE_VIBER				CDPI_PROTOCOL_VIBER
#define CDPI_SERVICE_YAHOO				CDPI_PROTOCOL_YAHOO
#define CDPI_PROTOCOL_P2P               1129

/* UPDATE UPDATE UPDATE UPDATE UPDATE UPDATE UPDATE UPDATE UPDATE */
///////////////////////////////////////////////////////////
#define CDPI_MYPROTOS_NUM                       	800
#define CDPI_PORTS_NUM		                       	256
#define CDPI_CUSTOMPROTOS_NUM                       10
///////////////////////////////////////////////////////////

#define CDPI_LAST_IMPLEMENTED_PROTOCOL			1500
#define CDPI_NON_HTTP                           	200

#define CDPI_MAX_SUPPORTED_PROTOCOLS (CDPI_LAST_IMPLEMENTED_PROTOCOL + 1)
#define CDPI_MAX_NUM_CUSTOM_PROTOCOLS                   (CDPI_NUM_BITS-CDPI_LAST_IMPLEMENTED_PROTOCOL)
#endif