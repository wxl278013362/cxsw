/**
 * @author qiao / https://github.com/qiao
 * @author mrdoob / http://mrdoob.com
 * @author alteredq / http://alteredqualia.com/
 * @author WestLangley / http://github.com/WestLangley
 * @author erich666 / http://erichaines.com
 */

// This set of controls performs orbiting, dollying (zooming), and panning.
// Unlike TrackballControls, it maintains the "up" direction _bindCamera.up (+Y by default).
//
//    Orbit - left mouse / touch: one-finger move
//    Zoom - middle mouse, or mousewheel / touch: two-finger spread or squish
//    Pan - right mouse, or left mouse + ctrl/metaKey, or arrow keys / touch: two-finger move
import * as THREE from "three";
import { CameraInteractionControl, InteractionType } from "./InteractionControl";
import { Scene3D } from "../core/Scene3D";
import { EventControl } from "../event/EventControl";

enum STATE { 
    NONE = - 1,
    ROTATE = 0,
    DOLLY = 1,
    PAN = 2,
    TOUCH_ROTATE = 3,
    TOUCH_DOLLY_PAN = 4
};
enum MouseButtons {
    LEFT = 0,
    MIDDLE = 1,
    RIGHT = 2,

};
/**
 * 相机控制器
 */
export class OrbitControls extends CameraInteractionControl {
    // Set to false to disable this control
    // public _enable = true;

    // "target" sets the location of focus, where the _bindCamera orbits around
    public target = new THREE.Vector3();

    // How far you can dolly in and out ( PerspectiveCamera only )
    public minDistance = 0;
    public maxDistance = Infinity;

    // How far you can zoom in and out ( OrthographicCamera only )
    public minZoom = 0;
    public maxZoom = Infinity;

    // How far you can orbit vertically, upper and lower limits.
    // Range is 0 to Math.PI radians.
    public minPolarAngle = 0; // radians
    public maxPolarAngle = Math.PI; // radians

    // How far you can orbit horizontally, upper and lower limits.
    // If set, must be a sub-interval of the interval [ - Math.PI, Math.PI ].
    public minAzimuthAngle = - Infinity; // radians
    public maxAzimuthAngle = Infinity; // radians

    // Set to true to enable damping (inertia)
    // If damping is enabled, you must call controls.update() in your animation loop
    public enableDamping = false;
    public dampingFactor = 0.25;

    // This option actually enables dollying in and out; left as "zoom" for backwards compatibility.
    // Set to false to disable zooming
    public enableZoom = true;
    public zoomSpeed = 1.0;

    // Set to false to disable rotating
    public enableRotate = true;
    public rotateSpeed = 0.5;

    // Set to false to disable panning
    public enablePan = true;
    public panSpeed = 1.0;
    public screenSpacePanning = false; // if true, pan in screen-space
    public keyPanSpeed = 7.0;    // pixels moved per arrow key push

    // Set to true to automatically rotate around the target
    // If auto-rotate is enabled, you must call controls.update() in your animation loop
    public autoRotate = false;
    public autoRotateSpeed = 2.0; // 30 seconds per round when fps is 60

    // Set to false to disable use of the keys
    public enableKeys = true;

    // The four arrow keys
    public keys = { LEFT: 37, UP: 38, RIGHT: 39, BOTTOM: 40};
    public operateKey = {W: 87, S: 83, A: 65, D: 68}

    // for reset
    public target0: THREE.Vector3;
    public position0: THREE.Vector3;
    public zoom0: number;

    private state = STATE.NONE;
    private EPS = 0.000001;

    // current position in spherical coordinates
    private spherical = new THREE.Spherical();
    private sphericalDelta = new THREE.Spherical();

    private scale = 1;
    private panOffset = new THREE.Vector3();
    private zoomChanged = false;

    private rotateStart = new THREE.Vector2();
    private rotateEnd = new THREE.Vector2();
    private rotateDelta = new THREE.Vector2();

    private panStart = new THREE.Vector2();
    private panEnd = new THREE.Vector2();
    private panDelta = new THREE.Vector2();

    private dollyStart = new THREE.Vector2();
    private dollyEnd = new THREE.Vector2();
    private dollyDelta = new THREE.Vector2();

    public changeEvent = { type: 'change' };

    /**
     * 监测记录相机userData.target的值
     */
    private _target: Array< number >; 
    constructor(scene3D: Scene3D, camera: THREE.OrthographicCamera | THREE.PerspectiveCamera) {
        super(scene3D, camera);
        // 类型设置
        this.type = InteractionType.Orbit;
        // 原本的代码
        this.target0 = this.target.clone();
        this.position0 = this._bindCamera.position.clone();
        this.zoom0 = this._bindCamera.zoom;

        //监测相机焦点属性值变化
        let scope = this;
        Object.defineProperty( this._bindCamera.userData, "target", {

            get: function(){
                return scope._target;
            },
            set: function( value ){
                scope._target = value;
                scope.target.fromArray( scope._target );
            }
        });

        // let domElement = this._scene3D.getContainer();
        let domElement = this._scene3D.getRenderer()?.domElement;
        if (domElement == undefined) return;
        this.dispose();
        domElement.addEventListener( 'contextmenu', (event) => this.onContextMenu(event), false );

        domElement.addEventListener( 'mousedown', (event) => this.onMouseDown(event), false );
        domElement.addEventListener( 'wheel', (event) => this.onMouseWheel(event), false );

        domElement.addEventListener( 'touchstart', (event) => this.onTouchStart(event), false );
        domElement.addEventListener( 'touchend', (event) => this.onTouchEnd(event), false );
        domElement.addEventListener( 'touchmove', (event) => this.onTouchMove(event), false );

        window.addEventListener( 'keydown', (event) => this.onKeyDown(event), false );
        window.addEventListener( 'mouseup', (event) => this.onMouseUp(event), false );

        // force an update at start
        this.update();
    }

    /**
     * 公共方法
     */
    public getPolarAngle() {
        return this.spherical.phi;
    };

    public getAzimuthalAngle() {
        return this.spherical.theta;
    };

    public saveState() {
        this.target0.copy( this.target );
        this.position0.copy( this._bindCamera.position );
        this.zoom0 = this._bindCamera.zoom;
    };

    public reset() {
        this.target.copy( this.target0 );
        this._bindCamera.position.copy( this.position0 );
        this._bindCamera.zoom = this.zoom0;

        this._bindCamera.updateProjectionMatrix();
        this.update();
        this.state = STATE.NONE;
    };
    
    // this method is exposed, but perhaps it would be better if we can make it private...
    public update() {

        let offset = new THREE.Vector3();
        // so camera.up is the orbit axis
        let quat = new THREE.Quaternion().setFromUnitVectors( this._bindCamera.up, new THREE.Vector3( 0, 1, 0 ) );
        // let quatInverse = quat.clone().inverse();
        //新版本
        let quatInverse = quat.clone().invert();

        let lastPosition = new THREE.Vector3();
        let lastQuaternion = new THREE.Quaternion();

        let position = this._bindCamera.position;

        offset.copy( position ).sub( this.target );

        // rotate offset to "y-axis-is-up" space
        offset.applyQuaternion( quat );

        // angle from z-axis around y-axis
        this.spherical.setFromVector3( offset );

        if ( this.autoRotate && this.state === STATE.NONE ) {
            this.rotateLeft( this.getAutoRotationAngle() );
        }

        this.spherical.theta += this.sphericalDelta.theta;
        this.spherical.phi += this.sphericalDelta.phi;

        // restrict theta to be between desired limits
        this.spherical.theta = Math.max( this.minAzimuthAngle, Math.min( this.maxAzimuthAngle, this.spherical.theta ) );

        // restrict phi to be between desired limits
        this.spherical.phi = Math.max( this.minPolarAngle, Math.min( this.maxPolarAngle, this.spherical.phi ) );

        this.spherical.makeSafe();


        this.spherical.radius *= this.scale;

        // restrict radius to be between desired limits
        this.spherical.radius = Math.max( this.minDistance, Math.min( this.maxDistance, this.spherical.radius ) );

        // move target to panned location
        this.target.add( this.panOffset );

        offset.setFromSpherical( this.spherical );

        // rotate offset back to "camera-up-vector-is-up" space
        offset.applyQuaternion( quatInverse );

        position.copy( this.target ).add( offset );
        this._bindCamera.lookAt( this.target );
        this._bindCamera.userData.target = this.target.toArray();

        if ( this.enableDamping === true ) {
            this.sphericalDelta.theta *= ( 1 - this.dampingFactor );
            this.sphericalDelta.phi *= ( 1 - this.dampingFactor );
            this.panOffset.multiplyScalar( 1 - this.dampingFactor );

        } else {
            this.sphericalDelta.set( 0, 0, 0 );
            this.panOffset.set( 0, 0, 0 );
        }

        this.scale = 1;

        if ( this.zoomChanged ||
            lastPosition.distanceToSquared( this._bindCamera.position ) > this.EPS ||
            8 * ( 1 - lastQuaternion.dot( this._bindCamera.quaternion ) ) > this.EPS ) {

            // this.dispatchEvent( changeEvent );
            lastPosition.copy( this._bindCamera.position );
            lastQuaternion.copy( this._bindCamera.quaternion );
            this.zoomChanged = false;

            return true;
        }
        return false;
    }

    public dispose() {
        // let domElement = this._scene3D.getContainer();
        let domElement = this._scene3D.getRenderer()?.domElement;
        if (domElement == undefined) return;
        domElement.removeEventListener( 'contextmenu', this.onContextMenu, false );
        domElement.removeEventListener( 'mousedown', this.onMouseDown, false );
        domElement.removeEventListener( 'wheel', this.onMouseWheel, false );

        domElement.removeEventListener( 'touchstart', this.onTouchStart, false );
        domElement.removeEventListener( 'touchend', this.onTouchEnd, false );
        domElement.removeEventListener( 'touchmove', this.onTouchMove, false );

        domElement.removeEventListener( 'mousemove', this.onMouseMove, false );
        domElement.removeEventListener( 'mouseup', this.onMouseUp, false );

        window.removeEventListener( 'keydown', this.onKeyDown, false );
        window.removeEventListener( 'mouseup', this.onMouseUp, false );
    };

    private getAutoRotationAngle() {
        return 2 * Math.PI / 60 / 60 * this.autoRotateSpeed;
    }

    private getZoomScale() {
        this._scene3D._enable = true;
        return Math.pow( 0.95, this.zoomSpeed );
    }

    private rotateLeft( angle: number ) {
        this.sphericalDelta.theta -= angle;
    }

    private rotateUp( angle: number ) {
        this.sphericalDelta.phi -= angle;
    }

    private panLeft(distance: number, _bindCameraMatrix: THREE.Matrix4) {
        let v = new THREE.Vector3();
        v.setFromMatrixColumn( _bindCameraMatrix, 0 ); // get X column of _bindCameraMatrix
        v.multiplyScalar( - distance );

        this.panOffset.add( v );
    };

    private panUp(distance: number, _bindCameraMatrix: THREE.Matrix4) {
        /**
        * 高俊潇注
        * 解决上下平移出现缩放的问题
        */
        let v = new THREE.Vector3();
        v.setFromMatrixColumn( _bindCameraMatrix, 1 ); // get Y column of objectMatrix
        v.multiplyScalar( distance );

        this.panOffset.add( v );
    };

    //相机前后移动
    private toAdvance(distance: number, _bindCameraMatrix: THREE.Matrix4) {
        let v = new THREE.Vector3();
       
        if ( this.screenSpacePanning === true ) {
            v.setFromMatrixColumn( _bindCameraMatrix, 1 );
        } else {
            v.setFromMatrixColumn( _bindCameraMatrix, 0 );
            v.crossVectors( this._bindCamera.up, v );
        }
        v.multiplyScalar( distance );
        this.panOffset.add( v );
    }

    // deltaX and deltaY are in pixels; right and down are positive
    private pan(deltaX: number, deltaY: number) {
        let offset = new THREE.Vector3();
        // let element = this._scene3D.getContainer();
        let element = this._scene3D.getRenderer()?.domElement;
        if (element == undefined) return;

        // @ts-ignore
        if ( this._bindCamera.isPerspectiveCamera ) {
            let perspectiveCamera = this._bindCamera as THREE.PerspectiveCamera;
            // perspective
            let position = perspectiveCamera.position;
            // rotation = TranslateBack * Rotate * TranslateForth

            offset.copy( position ).sub( this.target );
            let targetDistance = offset.length();

            // half of the fov is center to top of screen
            targetDistance *= Math.tan( ( perspectiveCamera.fov / 2 ) * Math.PI / 180.0 );

            // we use only clientHeight here so aspect ratio does not distort speed
            this.panLeft( 2 * deltaX * targetDistance / element.clientHeight, perspectiveCamera.matrix );
            this.panUp( 2 * deltaY * targetDistance / element.clientHeight, perspectiveCamera.matrix );
        }
        // @ts-ignore
        else if ( this._bindCamera.isOrthographicCamera ) {
            let orthographicCamera = this._bindCamera as THREE.OrthographicCamera;
            // orthographic
            this.panLeft( deltaX * ( orthographicCamera.right - orthographicCamera.left ) / orthographicCamera.zoom / element.clientWidth, orthographicCamera.matrix );
            this.panUp( deltaY * ( orthographicCamera.top - orthographicCamera.bottom ) / orthographicCamera.zoom / element.clientHeight, orthographicCamera.matrix );

        } else {
            // camera neither orthographic nor perspective
            console.warn( 'WARNING: OrbitControls.js encountered an unknown camera type - pan disabled.' );
            this.enablePan = false;
        }
    };

    //相机前后移动
    private front(deltaX: number, deltaY: number) {
        let offset = new THREE.Vector3();;
        // let element = this._scene3D.getContainer();
        let element = this._scene3D.getRenderer()?.domElement;
        if (element == undefined) return;

        // @ts-ignore
        if ( this._bindCamera.isPerspectiveCamera ) {
            let perspectiveCamera = this._bindCamera as THREE.PerspectiveCamera;
            // perspective
            let position = perspectiveCamera.position;
            // rotation = TranslateBack * Rotate * TranslateForth

            offset.copy( position ).sub( this.target );
            let targetDistance = offset.length();

            // half of the fov is center to top of screen
            targetDistance *= Math.tan( ( perspectiveCamera.fov / 2 ) * Math.PI / 180.0 );

            // we use only clientHeight here so aspect ratio does not distort speed
            this.toAdvance( 2 * deltaY * targetDistance / element.clientHeight, perspectiveCamera.matrix );

        }
        // @ts-ignore
        else if ( this._bindCamera.isOrthographicCamera ) {
            let orthographicCamera = this._bindCamera as THREE.OrthographicCamera;
            // orthographic
            this.panLeft( deltaX * ( orthographicCamera.right - orthographicCamera.left ) / orthographicCamera.zoom / element.clientWidth, orthographicCamera.matrix );
            this.panUp( deltaY * ( orthographicCamera.top - orthographicCamera.bottom ) / orthographicCamera.zoom / element.clientHeight, orthographicCamera.matrix );

        } else {
            // camera neither orthographic nor perspective
            console.warn( 'WARNING: OrbitControls.js encountered an unknown camera type - pan disabled.' );
            this.enablePan = false;
        }
    }

    private dollyIn( dollyScale: number ) {
        // @ts-ignore
        if ( this._bindCamera.isPerspectiveCamera ) {
            this.scale /= dollyScale;
        }
        // @ts-ignore
        else if ( this._bindCamera.isOrthographicCamera ) {
            this._bindCamera.zoom = Math.max( this.minZoom, Math.min( this.maxZoom, this._bindCamera.zoom * dollyScale ) );
            this._bindCamera.updateProjectionMatrix();
            this.zoomChanged = true;
        } else {
            console.warn( 'WARNING: OrbitControls.js encountered an unknown camera type - dolly/zoom disabled.' );
            this.enableZoom = false;
        }

        EventControl.dispatchEvent('MSG_ON_MODEL_ZOOM', {flag: 'add', num: this.scale}, 'all');
    }

    private dollyOut( dollyScale: number ) {
        // @ts-ignore
        if ( this._bindCamera.isPerspectiveCamera ) {
            this.scale *= dollyScale;
        }
        // @ts-ignore
        else if ( this._bindCamera.isOrthographicCamera ) {
            this._bindCamera.zoom = Math.max( this.minZoom, Math.min( this.maxZoom, this._bindCamera.zoom / dollyScale ) );
            this._bindCamera.updateProjectionMatrix();
            this.zoomChanged = true;
        } else {
            console.warn( 'WARNING: OrbitControls.js encountered an unknown camera type - dolly/zoom disabled.' );
            this.enableZoom = false;
        }

        EventControl.dispatchEvent('MSG_ON_MODEL_ZOOM', {flag: 'sub', num: this.scale}, 'all');
    }

    //
    // event callbacks - update the _bindCamera state
    //
    private handleMouseDownRotate( event: MouseEvent) {
        this.rotateStart.set( event.clientX, event.clientY );
    }

    private handleMouseDownPan( event: MouseEvent ) {
        this.panStart.set( event.clientX, event.clientY );
    }

    private handleMouseMoveRotate( event: MouseEvent ) {
        this.rotateEnd.set( event.clientX, event.clientY );
        // rotation = TranslateBack * Rotate * Translatefront
        /**
         * 高俊潇注
         * 根据起始位置来旋转
         */
        // this.rotateDelta.subVectors( this.rotateEnd, this.rotateStart );
        this.rotateDelta.subVectors( this.rotateEnd, this.rotateStart ).multiplyScalar( this.rotateSpeed );
        // let element = this._scene3D.getContainer();
        let element = this._scene3D.getRenderer()?.domElement;
        if (element == undefined) return;

         /**
         * 高俊潇注
         * 判断鼠标左右上下滑动
         */
        let X = event.clientX - this.rotateStart.x;
        let Y = event.clientY - this.rotateStart.y;

        //判断鼠标左右滑动
        if (Math.abs(X) > Math.abs(Y) && X > 0) {
            this.rotateLeft( 2 * Math.PI * this.rotateDelta.x / element.clientHeight ); // yes, height
        }
        else if (Math.abs(X) > Math.abs(Y) && X < 0) {
            this.rotateLeft( 2 * Math.PI * this.rotateDelta.x / element.clientHeight ); // yes, height
        }
        //判断鼠标鼠标上下滑动
        else if (Math.abs(Y) > Math.abs(X) && Y > 0) {
            this.rotateUp( 2 * Math.PI * this.rotateDelta.y / element.clientHeight );
        }
        else if (Math.abs(Y) > Math.abs(X) && Y < 0 ) {
            this.rotateUp( 2 * Math.PI * this.rotateDelta.y / element.clientHeight );
        }

        // this.rotateUp( 2 * Math.PI * this.rotateDelta.y / element.clientHeight );

        //开启渲染器
        this._scene3D._enable = true;

        this.rotateStart.copy( this.rotateEnd );
        this.update();
    }

    private handleMouseMovePan( event: MouseEvent ) {
        this.panEnd.set( event.clientX, event.clientY );

         /**
         * 高俊潇注
         * 根据平移起始点操作
         */
        // this.panDelta.subVectors( this.panEnd, this.panStart );
        this.panDelta.subVectors( this.panEnd, this.panStart ).multiplyScalar( this.panSpeed );
        this.pan( this.panDelta.x, this.panDelta.y );
        this.panStart.copy( this.panEnd );
        // this.update();
    }

    private handleMouseUp( event: MouseEvent ) {
    }

    private handleMouseWheel( event: WheelEvent ) {
        //设置鼠标的缩放回事件
        if ( event.deltaY < 0 ) {
            this.dollyOut( this.getZoomScale() );
            // EventControl.dispatchEvent('MSG_ON_MODEL_ZOOM', {flag: 'sub', num: 0.1}, 'all');

        } else if ( event.deltaY > 0 ) {
            this.dollyIn( this.getZoomScale() );
            // EventControl.dispatchEvent('MSG_ON_MODEL_ZOOM', {flag: 'add', num: 0.1}, 'all');
        }

        // let inter = this._scene3D.getInteractionManager();
        // if (inter != undefined) {
        //     console.log(this._bindCamera.position.distanceTo( this.target ));
        // }
    }

    private handleKeyDown( event: KeyboardEvent ) {
        switch ( event.keyCode ) {
            case this.keys.UP:
                this.pan( 0, this.keyPanSpeed );
                this.update();
                break;

            case this.keys.BOTTOM:
                this.pan( 0, - this.keyPanSpeed );
                this.update();
                break;

            case this.keys.LEFT:
            case this.operateKey.A:
                this.pan( this.keyPanSpeed, 0 );
                this.update();
                break;

            case this.keys.RIGHT:
            case this.operateKey.D:
                this.pan( - this.keyPanSpeed, 0 );
                this.update();
                break;

            case this.operateKey.W:
                this.front( 0, this.keyPanSpeed );
                this.update();
                break;
            case this.operateKey.S:
                this.front( 0, -this.keyPanSpeed );
                this.update();
                break;
        }
    }

    private handleTouchStartRotate( event: TouchEvent ) {
        this._scene3D.onWindowResize();
        this.rotateStart.set( event.touches[ 0 ].pageX, event.touches[ 0 ].pageY );
    }

    private handleTouchStartDollyPan( event: TouchEvent ) {
        //开启渲染器
        if ( this.enableZoom ) {
            let dx = event.touches[ 0 ].pageX - event.touches[ 1 ].pageX;
            let dy = event.touches[ 0 ].pageY - event.touches[ 1 ].pageY;
            let distance = Math.sqrt( dx * dx + dy * dy );
            this.dollyStart.set( 0, distance );
        }
        if ( this.enablePan ) {
            let x = 0.5 * ( event.touches[ 0 ].pageX + event.touches[ 1 ].pageX );
            let y = 0.5 * ( event.touches[ 0 ].pageY + event.touches[ 1 ].pageY );
            this.panStart.set( x, y );
        }
    }

    private handleTouchMoveRotate( event: TouchEvent ) {
        
        this.rotateEnd.set( event.touches[ 0 ].pageX, event.touches[ 0 ].pageY );
         /**
         * 高俊潇注
         */
        // this.rotateDelta.subVectors( this.rotateEnd, this.rotateStart )
        // let element = this._scene3D.getContainer();
        let element = this._scene3D.getRenderer()?.domElement;
        if (element == undefined) return;

        let distance = this.rotateEnd.distanceTo(this.rotateStart);
         if (distance > 5) {
            this.rotateDelta.subVectors( this.rotateEnd, this.rotateStart ).multiplyScalar( this.rotateSpeed );
            this.rotateLeft( 2 * Math.PI * this.rotateDelta.x / element.clientHeight ); // yes, height
            this.rotateUp( 2 * Math.PI * this.rotateDelta.y / element.clientHeight );
        } 

        // this.rotateDelta.subVectors( this.rotateEnd, this.rotateStart ).multiplyScalar( this.rotateSpeed );
        // this.rotateLeft( 2 * Math.PI * this.rotateDelta.x / element.clientHeight ); // yes, height
        // this.rotateUp( 2 * Math.PI * this.rotateDelta.y / element.clientHeight );

        // this.rotateLeft( 2 * Math.PI * this.rotateDelta.x / element.clientWidth * this.rotateSpeed );
        // rotating up and down along whole screen attempts to go 360, but limited to 180
        // this.rotateUp( 2 * Math.PI * this.rotateDelta.y / element.clientHeight * this.rotateSpeed );
        this._scene3D._enable = true;

        this.rotateStart.copy( this.rotateEnd );
        this.update();
    }

    private handleTouchMoveDollyPan( event: TouchEvent ) {
        this._scene3D._enable = true;
        if ( this.enableZoom ) {
            let dx = event.touches[ 0 ].pageX - event.touches[ 1 ].pageX;
            let dy = event.touches[ 0 ].pageY - event.touches[ 1 ].pageY;

            let distance = Math.sqrt( dx * dx + dy * dy );
            this.dollyEnd.set( 0, distance );
            this.dollyDelta.set( 0, Math.pow( this.dollyEnd.y / this.dollyStart.y, this.zoomSpeed ) );
            this.dollyIn( this.dollyDelta.y );
            this.dollyStart.copy( this.dollyEnd );
        }

        if ( this.enablePan ) {
            let x = 0.5 * ( event.touches[ 0 ].pageX + event.touches[ 1 ].pageX );
            let y = 0.5 * ( event.touches[ 0 ].pageY + event.touches[ 1 ].pageY );

            this.panEnd.set( x, y );
            this.panDelta.subVectors( this.panEnd, this.panStart ).multiplyScalar( this.panSpeed );
            this.pan( this.panDelta.x, this.panDelta.y );
            this.panStart.copy( this.panEnd );
        }
        this.update();
    }

    private handleTouchEnd( event: TouchEvent ) {
    }

    // private onWindowMouseUp(event: MouseEvent) {

    // }

    private onMouseDown( event: MouseEvent ) {
        if ( this.enable == false ) return;
        event.preventDefault();

        switch ( event.button ) {
            case MouseButtons.LEFT:
                if ( event.ctrlKey || event.metaKey ) {
                    if ( this.enablePan === false ) return;
                    this.handleMouseDownPan( event );
                    this.state = STATE.PAN;

                } else {
                    if ( this.enableRotate === false ) return;
                    this.handleMouseDownRotate( event );
                    this.state = STATE.ROTATE;
                }
                break;

            // case MouseButtons.MIDDLE:
            //     if ( this.enableZoom === false ) return;
            //     this.handleMouseDownDolly( event );
            //     this.state = STATE.DOLLY;
            //     break;

            case MouseButtons.RIGHT:
            case MouseButtons.MIDDLE:
                if ( this.enablePan === false ) return;
                this.handleMouseDownPan( event );
                this.state = STATE.PAN;

                break;
        }

        if ( this.state !== STATE.NONE ) {
            let domElement = this._scene3D.getRenderer()?.domElement;
            if (domElement == undefined) return;
            domElement.addEventListener( 'mousemove', (event) => this.onMouseMove(event), false );
            domElement.addEventListener( 'mouseup', (event) => this.onMouseUp(event), false );
            // this.dispatchEvent( this.startEvent );
        }
    }

    private onMouseMove( event: MouseEvent ) {
        if ( this.enable == false ) return;
        event.preventDefault();

        switch ( this.state ) {
            case STATE.ROTATE:
                if ( this.enableRotate === false ) return;
                this.handleMouseMoveRotate( event );
                break;

            // case STATE.DOLLY:
            //     if ( this.enableZoom === false ) return;
            //    this.handleMouseMoveDolly( event );
            //     break;

            case STATE.PAN:
                if ( this.enablePan === false ) return;
                this.handleMouseMovePan( event );
                break;
        }
    }

    private onMouseUp( event: MouseEvent ) {
        if ( this.enable == false ) return;
        event.preventDefault();
        this.handleMouseUp( event );
        let domElement = this._scene3D.getRenderer()?.domElement;
        if (domElement == undefined) return;
        
        domElement.removeEventListener( 'mousemove', this.onMouseMove, false );
        domElement.removeEventListener( 'mouseup', this.onMouseUp, false );

        // this.dispatchEvent( this.endEvent );
        this.state = STATE.NONE;

        this._scene3D._enable = false;
    }

    private onMouseWheel( event: WheelEvent ) {
        if ( this.enable == false || this.enableZoom === false || ( this.state !== STATE.NONE && this.state !== STATE.ROTATE ) ) return;
        event.preventDefault();
        event.stopPropagation();

        this.handleMouseWheel( event );

        // this.dispatchEvent( this.endEvent );
    }

    private onKeyDown( event: KeyboardEvent ) {
        if ( this.enable == false || this.enableKeys === false || this.enablePan === false ) return;
        this.handleKeyDown( event );
    }

    private onTouchStart( event: TouchEvent ) {
        if ( this.enable == false ) return;
        event.preventDefault();

        switch ( event.touches.length ) {
            case 1:    // one-fingered touch: rotate
                if ( this.enableRotate === false ) return;
                this.handleTouchStartRotate( event );
                this.state = STATE.TOUCH_ROTATE;
                break;

            case 2:    // two-fingered touch: dolly-pan
                if ( this.enableZoom === false && this.enablePan === false ) return;
                this.handleTouchStartDollyPan( event );
                this.state = STATE.TOUCH_DOLLY_PAN;
                break;

            default:
                this.state = STATE.NONE;
        }

        if ( this.state !== STATE.NONE ) {
            // this.dispatchEvent( this.startEvent );
        }
    }

    private onTouchMove( event: TouchEvent ) {
        if ( this.enable == false ) return;

        event.preventDefault();
        event.stopPropagation();

        switch ( event.touches.length ) {
            case 1: // one-fingered touch: rotate
                if ( this.enableRotate === false ) return;
                if ( this.state !== STATE.TOUCH_ROTATE ) return; // is this needed?

                this.handleTouchMoveRotate( event );
                break;

            case 2: // two-fingered touch: dolly-pan
                if ( this.enableZoom === false && this.enablePan === false ) return;
                if ( this.state !== STATE.TOUCH_DOLLY_PAN ) return; // is this needed?

                this.handleTouchMoveDollyPan( event );
                break;

            default:
            this.state = STATE.NONE;
        }
    }

    private onTouchEnd( event: TouchEvent ) {
        if ( this.enable == false ) return;
        this.handleTouchEnd( event );
        // this.dispatchEvent( this.endEvent );
        this.state = STATE.NONE;
        // this._scene3D._enable = false;
    }

    private onContextMenu( event: MouseEvent ) {
        if ( this.enable == false ) return;
        event.preventDefault();
    }
}