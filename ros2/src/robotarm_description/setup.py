import glob
from setuptools import find_packages, setup

package_name = 'robotarm_description'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/urdf', ['urdf/robotarm.urdf', 'urdf/robotarm_wall.urdf']),
        ('share/' + package_name + '/launch', [
            'launch/debug_wall.launch.py',
            'launch/display.launch.py',
            'launch/display_wall.launch.py',
            'launch/replay_wall.launch.py',
            'launch/view.launch.py',
            'launch/view_wall.launch.py',
        ]),
        ('share/' + package_name + '/config', ['config/display.rviz', 'config/display_wall.rviz']),
        ('share/' + package_name + '/meshes', glob.glob('meshes/*.stl')),
    ],
    install_requires=['setuptools', 'paho-mqtt'],
    zip_safe=True,
    maintainer='p-keminer',
    maintainer_email='p-keminer@users.noreply.github.com',
    description='Adeept 5-DOF Roboterarm ROS 2 Beschreibung mit MQTT-Bridge',
    license='MIT',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'debug_plot = robotarm_description.debug_plot:main',
            'debug_recorder = robotarm_description.debug_recorder:main',
            'imu_replayer = robotarm_description.imu_replayer:main',
            'live_monitor = robotarm_description.live_monitor:main',
            'mqtt_bridge = robotarm_description.mqtt_bridge:main',
        ],
    },
)
