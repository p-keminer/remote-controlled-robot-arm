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
        ('share/' + package_name + '/launch', ['launch/display.launch.py', 'launch/view.launch.py', 'launch/view_wall.launch.py']),
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
            'mqtt_bridge = robotarm_description.mqtt_bridge:main',
        ],
    },
)
