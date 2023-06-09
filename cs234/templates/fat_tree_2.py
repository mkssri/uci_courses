"""Custom topology example

Two directly connected switches plus a host for each switch:

   host --- switch --- switch --- host

Adding the 'topos' dict with a key/value pair to generate our newly defined
topology enables one to pass in '--topo=mytopo' from the command line.
"""

from mininet.topo import Topo

class MyTopo( Topo ):
    "Simple topology example."

    def build( self ):
        "Create custom topo."

        # Add hosts 
        h1 = self.addHost( 'h1' )
        h2 = self.addHost( 'h2' )
        h3 = self.addHost( 'h3' )
        h4 = self.addHost( 'h4' )
        h5 = self.addHost( 'h5' )
        h6 = self.addHost( 'h6' )
        h7 = self.addHost( 'h7' )
        h8 = self.addHost( 'h8' )


        #Add switches
        s1 = self.addSwitch( 's1' )
        s2 = self.addSwitch( 's2' )
        s3 = self.addSwitch( 's3' )
        s4 = self.addSwitch( 's4' )
        s5 = self.addSwitch( 's5' )
        s6 = self.addSwitch( 's6' )
        s7 = self.addSwitch( 's7' )
        s8 = self.addSwitch( 's8' )
        s9 = self.addSwitch( 's9' )
        s10 = self.addSwitch( 's10' )


        #Add Links

        # hosts to switches
        self.addLink( h1, s7 )
        self.addLink( h2, s7 )
        self.addLink( h3, s8 )
        self.addLink( h4, s8 )
        self.addLink( h5, s9 )
        self.addLink( h6, s9 )
        self.addLink( h7, s10 )
        self.addLink( h8, s10 )


        # switches to switches
        self.addLink( s7, s3 )
        self.addLink( s7, s4 )
        self.addLink( s8, s3 )
        self.addLink( s8, s4 )
        self.addLink( s9, s5 )
        self.addLink( s9, s6 )
        self.addLink( s10, s5 )
        self.addLink( s10, s6 )


        # switches to switches
        self.addLink( s3, s1 )
        self.addLink( s5, s1 )
        self.addLink( s4, s2 )
        self.addLink( s6, s2 )


topos = { 'mytopo': ( lambda: MyTopo() ) }