ECO32
========

eco32sim
-------

1. Download an build the eco32 project from github:

.. code-block:: console

 $ cd ~/
 $ git clone https://github.com/hgeisse/eco32
 $ cd eco32
 $ make


2. Run or1ksim:

.. code-block:: console

 $ ~/eco32/build/bin/sim -l barebox.bin -a 0x00400000 -m 32 -s 2 -t 0 -d <diskimage>
