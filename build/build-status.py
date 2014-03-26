#! /usr/bin/env python

# Programs that are runnable.
ns3_runnable_programs = ['ns3-dev-aodv-debug', 'ns3-dev-csma-bridge-debug', 'ns3-dev-csma-bridge-one-hop-debug', 'ns3-dev-buildings-pathloss-profiler-debug', 'ns3-dev-config-store-save-debug', 'ns3-dev-main-callback-debug', 'ns3-dev-sample-simulator-debug', 'ns3-dev-main-ptr-debug', 'ns3-dev-main-random-variable-debug', 'ns3-dev-sample-random-variable-debug', 'ns3-dev-main-test-sync-debug', 'ns3-dev-csma-one-subnet-debug', 'ns3-dev-csma-broadcast-debug', 'ns3-dev-csma-packet-socket-debug', 'ns3-dev-csma-multicast-debug', 'ns3-dev-csma-raw-ip-socket-debug', 'ns3-dev-csma-ping-debug', 'ns3-dev-csma-star-debug', 'ns3-dev-dsdv-manet-debug', 'ns3-dev-emu-udp-echo-debug', 'ns3-dev-emu-ping-debug', 'ns3-dev-li-ion-energy-source-debug', 'ns3-dev-main-simple-debug', 'ns3-dev-netfilter-three-node-debug', 'ns3-dev-netfilter-add-remove-debug', 'ns3-dev-netfilter-header-mangle-debug', 'ns3-dev-ipv4-static-nat-example-debug', 'ns3-dev-ipv4-dynamic-nat-example-debug', 'ns3-dev-lena-cqi-threshold-debug', 'ns3-dev-lena-dual-stripe-debug', 'ns3-dev-lena-fading-debug', 'ns3-dev-lena-intercell-interference-debug', 'ns3-dev-lena-pathloss-traces-debug', 'ns3-dev-lena-profiling-debug', 'ns3-dev-lena-rem-debug', 'ns3-dev-lena-rem-sector-antenna-debug', 'ns3-dev-lena-rlc-traces-debug', 'ns3-dev-lena-simple-debug', 'ns3-dev-lena-simple-epc-debug', 'ns3-dev-mesh-debug', 'ns3-dev-main-grid-topology-debug', 'ns3-dev-main-random-topology-debug', 'ns3-dev-main-random-walk-debug', 'ns3-dev-simple-distributed-debug', 'ns3-dev-third-distributed-debug', 'ns3-dev-nms-p2p-nix-distributed-debug', 'ns3-dev-dumbbell-animation-debug', 'ns3-dev-grid-animation-debug', 'ns3-dev-star-animation-debug', 'ns3-dev-wireless-animation-debug', 'ns3-dev-main-packet-header-debug', 'ns3-dev-main-packet-tag-debug', 'ns3-dev-red-tests-debug', 'ns3-dev-droptail_vs_red-debug', 'ns3-dev-nix-simple-debug', 'ns3-dev-nms-p2p-nix-debug', 'ns3-dev-simple-point-to-point-olsr-debug', 'ns3-dev-olsr-hna-debug', 'ns3-dev-main-attribute-value-debug', 'ns3-dev-main-propagation-loss-debug', 'ns3-dev-jakes-propagation-model-example-debug', 'ns3-dev-adhoc-aloha-ideal-phy-debug', 'ns3-dev-adhoc-aloha-ideal-phy-matrix-propagation-loss-model-debug', 'ns3-dev-adhoc-aloha-ideal-phy-with-microwave-oven-debug', 'ns3-dev-tap-csma-debug', 'ns3-dev-tap-csma-virtual-machine-debug', 'ns3-dev-tap-wifi-virtual-machine-debug', 'ns3-dev-tap-wifi-dumbbell-debug', 'ns3-dev-gnuplot-example-debug', 'ns3-dev-topology-read-debug', 'ns3-dev-uan-cw-example-debug', 'ns3-dev-uan-rc-example-debug', 'ns3-dev-virtual-net-device-debug', 'ns3-dev-wifi-phy-test-debug', 'ns3-dev-wimax-ipv4-debug', 'ns3-dev-wimax-multicast-debug', 'ns3-dev-wimax-simple-debug', 'ns3-dev-udp-echo-debug', 'ns3-dev-energy-model-example-debug', 'ns3-dev-dynamic-global-routing-debug', 'ns3-dev-static-routing-slash32-debug', 'ns3-dev-global-routing-slash32-debug', 'ns3-dev-global-injection-slash32-debug', 'ns3-dev-simple-global-routing-debug', 'ns3-dev-simple-alternate-routing-debug', 'ns3-dev-mixed-global-routing-debug', 'ns3-dev-simple-routing-ping6-debug', 'ns3-dev-manet-routing-compare-debug', 'ns3-dev-simple-error-model-debug', 'ns3-dev-realtime-udp-echo-debug', 'ns3-dev-socket-bound-static-routing-debug', 'ns3-dev-socket-bound-tcp-static-routing-debug', 'ns3-dev-icmpv6-redirect-debug', 'ns3-dev-ping6-debug', 'ns3-dev-radvd-debug', 'ns3-dev-radvd-two-prefix-debug', 'ns3-dev-test-ipv6-debug', 'ns3-dev-fragmentation-ipv6-debug', 'ns3-dev-loose-routing-ipv6-debug', 'ns3-dev-hello-simulator-debug', 'ns3-dev-first-debug', 'ns3-dev-second-debug', 'ns3-dev-third-debug', 'ns3-dev-fourth-debug', 'ns3-dev-fifth-debug', 'ns3-dev-sixth-debug', 'ns3-dev-mixed-wireless-debug', 'ns3-dev-wifi-clear-channel-cmu-debug', 'ns3-dev-wifi-wired-bridging-debug', 'ns3-dev-simple-wifi-frame-aggregation-debug', 'ns3-dev-multirate-debug', 'ns3-dev-wifi-blockack-debug', 'ns3-dev-ofdm-validation-debug', 'ns3-dev-wifi-hidden-terminal-debug', 'ns3-dev-tcp-nsc-lfn-debug', 'ns3-dev-tcp-nsc-zoo-debug', 'ns3-dev-tcp-star-server-debug', 'ns3-dev-star-debug', 'ns3-dev-tcp-bulk-send-debug', 'ns3-dev-udp-client-server-debug', 'ns3-dev-udp-trace-client-server-debug', 'ns3-dev-ns2-mobility-trace-debug', 'ns3-dev-object-names-debug', 'ns3-dev-matrix-topology-debug', 'ns3-dev-wifi-example-sim-debug', 'ns3-dev-scratch-simulator-debug', 'ns3-dev-subdir-debug']

# Scripts that are runnable.
ns3_runnable_scripts = ['csma-bridge.py', 'sample-simulator.py', 'wifi-olsr-flowmon.py', 'tap-csma-virtual-machine.py', 'tap-wifi-virtual-machine.py', 'simple-routing-ping6.py', 'realtime-udp-echo.py', 'first.py', 'mixed-wireless.py']
