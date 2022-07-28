.. highlight:: none

Connecting to Github from Cobalts
=================================

This page describes how to connect to github repositories from the cobalts.
The communication protocol used by both github and the cobalts is ssh. 
The easiest way to connect to github from the cobalts is to create a public/private key pair
on your laptop and then use that same key pair to connect to both the cobalts and github using
an ssh-agent. 

Generating a key pair on your laptop
------------------------------------

Start on you laptop and make sure you have an ssh-agent running.
On MacOS an agent should already be running.
On Linux, most desktop environments will automatically start an agent. 
To verify an agent is running on your laptop, you can run the following command:

.. code-block:: console

    $ echo $SSH_AGENT_PID $SSH_AUTH_SOCK 

If an agent is running, you should get output that looks like this:

::

    263673 /tmp/ssh-XXXXXX4GogBF/agent.263672

If you do not see any output for this command, stop and install an ssh agent.
Installing an agent is beyond the scope of this document, please consult the documentation
for your linux distribution and/or desktop environment.

Once you have confirmed that you have an ssh-agent running, the next step is to generate a key. 
On your laptop run:

.. code-block:: console

    $ ssh-keygen -t ed25519 -C "<YourUsername>@<YourLaptopName>"

The ``-C`` option is a comment for your own record keeping, what you type there will 
not effect the functionality of your key. It is tradition to write your user name and
the hostname of your laptop here. This helps you keep track of which key is which 
if you have multiple keys generated from multiple devices.

You will be prompted to enter the filename for your private key, the default is fine.
You will also be prompted to enter a passphrase. **DO NOT LEAVE THIS EMPTY**.
Please enter a secure passphrase, something that will be hard for someone to guess.
The output should look like this:

::

    Generating public/private ed25519 key pair.
    Enter file in which to save the key (/home/username/.ssh/id_ed25519): 
    Enter passphrase (empty for no passphrase): 
    Enter same passphrase again: 
    Your identification has been saved in /home/username/.ssh/id_ed25519
    Your public key has been saved in /home/username/.ssh/id_ed25519.pub
    The key fingerprint is:
    SHA256:qgeTLaHoFn9xH5uyLu5IpI7rvV1SUtkjQZZSXrgPo9E username@LaptopName
    The key's randomart image is:
    +--[ED25519 256]--+
    |      o=o.       |
    |     .oo=        |
    |      o=.o       |
    |    ...E. .      |
    | . o =o.S        |
    |..+ =o+o o       |
    |..o. =+.. +      |
    |ooo.o++. +       |
    |=+ +*=ooo        |
    +----[SHA256]-----+

Now lest take a look at your ``.ssh`` directory:

.. code-block:: console

    $ ls -l ~/.ssh

you should see that ``ssh-keygen`` created two files: 

::

    .rw------- 444 username 28 Jun 13:24 id_ed25519
    .rw-r--r--  91 username 28 Jun 13:24 id_ed25519.pub

``id_ed25519`` is your private key, do not ever share the contents of this file with
anyone and do not ever copy this file from one device to another. 
``id_ed25519.pub`` is the public key, this is what you will be sharing with servers that 
you want to connect to.

Connecting to github with your key
----------------------------------

Now we need to let github know about the public key so we can access our repositories.
Go to your github's setting's keys page: https://github.com/settings/keys.
Or click on the menu on the top right corner of github and go to ``settings`` -> ``SSH and GPG keys``.

Click on the green button that says ``New SSH key``. You should see two text boxes:

.. image:: ssh_new.png
    :width: 90%
    :align: center

In ``Title`` you can type whatever you want, most likely you will want to type
the same thing as the comment when you ran ``ssh-keygen`` above.
In the ``key`` box copy the contents of ``~/.ssh/id_ed25519.pub`` and 
press the ``Add SSH key`` button.

You should see that your key has been successfully added to github, it will look like this:

.. image:: ssh_keys.png
    :width: 90%
    :align: center

At this point you should be able to authenticate with github on your laptop using you new key.

.. code-block:: console

    $ ssh -T git@github.com
    The authenticity of host 'github.com (140.82.113.4)' can't be established.
    ED25519 key fingerprint is SHA256:+DiY3wvvV6TuJJhbpZisF/zLDA0zPMSvHdkr4UvCOqU.
    This key is not known by any other names
    Are you sure you want to continue connecting (yes/no/[fingerprint])? yes
    Warning: Permanently added 'github.com' (ED25519) to the list of known hosts.
    Enter passphrase for key '/home/username/.ssh/id_rsa': 
    Hi username! You've successfully authenticated, but GitHub does not provide shell access.

Make sure that the fingerprint matches what github 
`posts on their website <https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/githubs-ssh-key-fingerprints>`_
and then enter the passphrase for the key as prompted.
If you see the message that you authenticated then you have successfully setup your key.

Logging to in the cobalts with the key pair
-------------------------------------------

Now that we have a key pair we need to transfer the public key to pub so that we
can ssh into pub and the cobalts. On your laptop run

.. code-block:: console

    $ ssh-copy-id -i ~/.ssh/id_ed25519.pub <yourusername>@pub.icecube.wisc.edu

In order to copy your public key to pub you will need your IceCube LDAP password and you
will be prompted for this.

Now you should be able to login to pub with your key pair instead of your password. Try running

.. code-block:: console

    $ ssh <yourusername>@pub.icecube.wisc.edu

If you are prompted for a passphrase, then your key pair is working.
If you are instead prompted for a password, that means that something is wrong with your key pair,
this needs to be fixed before continuing.

At this point every time you log into pub you will be prompted for your passphrase,
this can get annoying, so let's add our key to the agent when it is used.
Edit your  ``~/.ssh/config`` and add the following line:

::

    AddKeysToAgent yes

In order for this to work you need to set the permissions correctly on the config file

.. code-block:: console

    $ chmod 600 ~/.ssh/config

Now you will be prompted to enter your passphrase the first time you try to log into pub,
but all subsequent logins will not need the passphrase. 

Now we can login to pub with our key pair, but to get any actual work done we need to login
to the cobalts. However, if we try to login to the cobalts from pub at this point we will get a password
prompt. If we enable agent forwarding than the key in the agent on our laptop can also be used to when 
we login to cobalt from pub.

Logout of pub and edit ``~/.ssh/config`` on your laptop. Add the following lines:

::

    Host pub.icecube.wisc.edu     
        ForwardAgent yes

With this in your laptop's ssh config you should be able first login to pub and then login
to cobalts without a passphrase prompt. This also means that your key pair is also available
for use with github on cobalts.

.. Warning::

    You may be tempted to use a wildcard like ``Host *`` to just apply this setting to all SSH connections. 
    That's not really a good idea, as you'd be sharing your local SSH keys with every server you SSH into. 
    They won't have direct access to the keys, but they will be able to use them as you while 
    the connection is established. 
    **You should only add servers you trust and that you intend to use with agent forwarding.**

Now in order to have your key forwarded from pub to cobalt you need to enable forwarding on pub.
On pub edit ``~/.ssh/config`` and add the following line:

::

    Host cobalt*
        ForwardAgent yes

and once again set the permissions on your ssh config:

.. code-block:: console

   pub$ chmod 700 ~/.ssh/config

Now that github has your public key, you should be able to login to cobalt and authenticate with github:

.. code-block:: console

    laptop$ ssh <yourusernamne>@pub.icecube.wisc.edu
    pub$ ssh cobalt
    cobalt01$ $ ssh -T git@github.com
    Hi username! You've successfully authenticated, but GitHub does not provide shell access.

.. Warning::

    If you want to login from more than one device, you should create a keypair
    for each device by repeating the instructions above for each device. 
    Do **NOT** copy your private key from one device to another.

Additional Tips
---------------

If you get tired of typing your username and the full hostname of pub into ssh
you can create an alias in ``~/.ssh/config`` so all you have to type is ``ssh pub``.

::

    Host pub
        HostName %h.icecube.wisc.edu
        User <username>
        ForwardAgent yes

You can make things even more convenient by setting up a proxy jump to login
directly to the cobalts from your laptop

::

    Host cobalt*
        ProxyJump pub.icecube.wisc.edu
        User <username>
        ForwardAgent yes

You can now login directly to the cobalts from your laptop

.. code-block:: console

    $ ssh cobalt

Another Option that might be helpful is to add a keep alive signal to prevent
being disconnected when connecting through firewalls that may drop idle connections

::

    ServerAliveInterval 120
